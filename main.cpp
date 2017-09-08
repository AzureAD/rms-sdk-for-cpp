// mip_upe_hello.cpp : Defines the entry point for the console application.
//
#include <stdio.h>

#include "api/profile.h"
#include "api/mip_error.h"
#include "api/label.h"
#include "api/action.h"
#include "api/protect_by_template_action.h"
#include "api/metadata_action.h"
#include "api/content_marking_action.h"
#include "api/water_marking_action.h"
#include "api/policy_engine.h"

#include <memory>
#include <vector>
#include <iostream>
#include <iomanip>
#include <map>
#include <mutex>
#include <condition_variable>

using namespace mip;
using namespace std;
using std::cin;
using std::condition_variable;
using std::cout;
using std::mutex;
using std::shared_ptr;
using std::string;
using std::vector;

// Global Variables:
shared_ptr<IProfile> mip_profile;
shared_ptr<IPolicyEngine> mip_engine;
mutex mtx;
condition_variable cv;
bool ready = false;
bool processed = false;
string nextLabelId = "";

class ExecutionState : public mip::IExecutionState {
    static vector<pair<string, string>> GetEmptyMetadata() {
        vector<pair<string, string>> metaData;
        return metaData;
    }
    
    virtual std::string GetNewSensitivityLabelId() const override
    {
        return nextLabelId;
    }
    
    virtual bool IsDowngradeJustified() const override
    {
        return false;
    }
    
    virtual std::vector<std::pair<std::string, std::string>> GetContentMetadata(
                                                                                const std::vector<std::string>& names,
                                                                                const std::vector<std::string>& name_prefixes) const override {
        return GetEmptyMetadata();
    }
    
    virtual std::string GetOwner() const override {
        return "john.smith@contoso.com";
    }
    
    virtual std::string GetApplicationName() const override {
        return "SampleApp";
    }
};

void printLabels(vector<shared_ptr<ILabel>> policies) {
    for (int i = 0; i < policies.size(); i++) {
        cout << "[" << i << "]" << "\t" << setw(-20) << policies[i]->GetName();
        cout << setw(70 - policies[i]->GetName().size()) << policies[i]->GetId() << endl;
    }
}

void printActions(vector<shared_ptr<IAction>> actions) {
    for (int i = 0; i < actions.size(); i++) {
        cout << "[" << i << "]\t";
        switch (actions[i]->GetType()) {
            case IAction::ActionType::ACTION_TYPE_PROTECT_ADHOC:
                cout << "Protect Adhoc Action";
                break;
            case IAction::ActionType::ACTION_TYPE_PROTECT_BY_TEMPLATE:
                cout << "Protect by Template Action";
                break;
            case IAction::ActionType::ACTION_TYPE_PROTECT_DO_NOT_FORWARD:
                cout << "Do Not Forward Action";
                break;
            case IAction::ActionType::ACTION_TYPE_METADATA:
                cout << "Metadata Action";
                break;
            case IAction::ActionType::ACTION_TYPE_CONTENT_MARKING:
                cout << "Content Marking Action";
                break;
            case IAction::ActionType::ACTION_TYPE_JUSTIFY:
                cout << "Justify Action";
                break;
            case IAction::ActionType::ACTION_TYPE_WATER_MARKING:
                cout << "Water Marking Action";
                break;
            default:
                break;
        }
        cout << setw(40) << actions[i]->GetId() << endl;
    }
}

namespace {
    static const string someXml =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n <SyncFile xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"> <TenantId>095560df-95e2-4c6f-bcd2-91b16d9bcb75</TenantId> <Type>InfomationProtectionPolicies</Type> <Timestamp>2017-06-10T23:59:59.9999999Z</Timestamp> <FileId>F9A8B5F184AD66D31C3CFECA7784256B2798D98A310A032D2E7BEAA7A50F347572F988BF86F141AF91AB2D7CD011DB47</FileId> <Content> <labels> <displayName locale=\"en-US\">Sensitivity</displayName> <description locale=\"en-US\">Data Sensitivity labels describe the level of risk and exposure to users inside and outside the company to this information</description> <settings> <setting key=\"attachmentAction\" value=\"Off\" /> <setting key=\"requireDowngradeJustification\" value=\"true\" /> <setting key=\"defaultLabelId\" value=\"d77edb09-ae2f-48b0-a7cb-77e50345830b\" /> </settings> <label id=\"3df71041-85be-479d-b153-8acaec1fe5c7\"> <displayName locale=\"en-US\">Non-Business</displayName> <description locale=\"en-US\">Non-Business data which does not belong to Contoso. Data is not encrypted and cannot be tracked or revoked. Do not use Non-Business to classify any personal data which is collected by or which belongs to Contoso. Such content should be marked as either Confidential or Highly Confidential.</description> <settings> <setting key=\"order\" value=\"1\" /> <setting key=\"color\" value=\"#737373\" /> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> <label id=\"69439d00-5cc7-44d2-ab0b-96af4bb0e7e9\"> <displayName locale=\"en-US\">Public</displayName> <description locale=\"en-US\">Business data specifically prepared and approved for public consumption. Data is not encrypted, and cannot be tracked or revoked.</description> <settings> <setting key=\"order\" value=\"2\" /> <setting key=\"color\" value=\"#317100\" /> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> <label id=\"d77edb09-ae2f-48b0-a7cb-77e50345830b\"> <displayName locale=\"en-US\">General</displayName> <description locale=\"en-US\">Business data which is NOT meant for public consumption. However, this can be shared with internal employees, business guests and external partners as needed. Data is not encrypted, and cannot be tracked or revoked.</description> <settings> <setting key=\"order\" value=\"3\" /> <setting key=\"color\" value=\"#0078D7\" /> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> <label id=\"6a975473-dc1b-46bb-9a1e-02615c031853\"> <displayName locale=\"en-US\">Confidential</displayName> <description locale=\"en-US\">Sensitive business data which could cause business harm if over-shared. Recipients are trusted and get full delegation rights. Data is encrypted. Data owners can track and revoke content.</description> <settings> <setting key=\"order\" value=\"4\" /> <setting key=\"color\" value=\"#FF8C00\" /> <setting key=\"viewOnly\" value=\"false\" /> </settings> <labels> <description locale=\"en-US\">Sensitive business data which could cause business harm if over-shared. Recipients are trusted and get full delegation rights. Data is encrypted. Data owners can track and revoke content.</description> <label id=\"c8deb7ae-676d-41d3-b0b3-51ee9c4d9b7b\"> <displayName locale=\"en-US\">Recipients Only</displayName> <description locale=\"en-US\">Data is classified and protected. Recipients can view but cannot forward, print or copy the content.</description> <settings> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> <label id=\"0d23bc70-d65e-429c-99d4-a2176d198741\"> <displayName locale=\"en-us\">Contoso Executives and Staff</displayName> <description locale=\"en-us\">Data is classified &amp; protected. Contoso Executive Staff can edit, forward, reply, print and unprotect. Direct reports can only edit and reply.</description> <settings> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> <label id=\"7de135f8-ae65-4421-b996-b9c2b8aae5a7\"> <displayName locale=\"en-US\">Contoso FTE</displayName> <description locale=\"en-US\">Data is classified and protected. Contoso Full Time Employees (FTE) can edit, reply, forward and print. Recipient can unprotect content with proper justification.</description> <settings> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> <label id=\"bd621434-11b6-4854-84a0-5e9df216fb7c\"> <displayName locale=\"en-US\">Contoso Extended</displayName> <description locale=\"en-US\">Data is classified and protected. Contoso Full Time Employees (FTE) and non-employees can edit, reply, forward and print. Recipient can unprotect content with the right justification.</description> <settings> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> <label id=\"5d7759ff-504a-433f-8023-8780e280f4fb\"> <displayName locale=\"en-US\">Any User (No Protection)</displayName> <description locale=\"en-US\">Data is classified as Confidential but is NOT protected. The option must be used with care with the right justification. Justification &amp; usage is audited.</description> <settings> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> <label id=\"a3d4a808-51c7-4b03-9736-2c163a738577\"> <displayName locale=\"en-US\">IPRMS Team: Test Label</displayName> <description locale=\"en-US\">This label is designed for the IPRMS team to test new features and bug fixes.</description> <settings> <setting key=\"viewOnly\" value=\"true\" /> </settings> </label> </labels> </label> <label id=\"98c836a2-1532-4e89-877b-7dc674798756\"> <displayName locale=\"en-US\">Highly Confidential</displayName> <description locale=\"en-US\">Very sensitive business data which would certainly cause business harm if over-shared. Recipients do NOT get delegation rights. Data is encrypted and rights are enforced. Data owners can track and revoke content.</description> <settings> <setting key=\"order\" value=\"5\" /> <setting key=\"color\" value=\"#A80000\" /> <setting key=\"viewOnly\" value=\"false\" /> </settings> <labels> <label id=\"21674434-b332-4647-948b-7882b4a55807\"> <displayName locale=\"en-US\">Recipients Only</displayName> <description locale=\"en-US\">Data is classified and protected. Contoso Full Time Employees (FTE) can edit, reply, forward and print. Recipient can unprotect content with proper justification.</description> <settings> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> <label id=\"8ef3c83a-d6c2-4a49-aa37-ae5ef3739ce6\"> <displayName locale=\"en-US\">Contoso Executives and Staff</displayName> <description locale=\"en-US\">Data is classified and protected. Contoso Executive Staff and Directs can view, edit, and reply.</description> <settings> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> <label id=\"5fb6eaa5-56a9-4aeb-8719-f45f81471183\"> <displayName locale=\"en-US\">Contoso FTE</displayName> <description locale=\"en-US\">Data is classified and protected. Contoso Executive Staff and Directs can view, edit, and reply.</description> <settings> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> <label id=\"d83c8aa3-afac-4478-9759-46608805e2bd\"> <displayName locale=\"en-US\">Contoso Extended</displayName> <description locale=\"en-US\">Data is classified and protected. Contoso Full Time Employees (FTE) and non employees can view, edit and reply.</description> <settings> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> <label id=\"5d7759ff-504a-433f-8023-8780e280f4fb\"> <displayName locale=\"en-US\">Any User (No Protection)</displayName> <description locale=\"en-US\">Data is classified as Highly Confidential but is NOT protected. The option must be used with care with the right justification. Justification &amp; usage is audited.</description> <settings> <setting key=\"viewOnly\" value=\"false\" /> </settings> </label> </labels> </label> </labels> <policies> <policy id=\"fc785c15-e648-4673-b207-490455aff114\" mode=\"Sensitivity\"> <rules> <rule name=\"ConfidentialApplyFooter\" id=\"746882e6-dc45-479a-b3cf-77c70f9bce51\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"6a975473-dc1b-46bb-9a1e-02615c031853\" /> </keyValues> </containsClassification> </condition> <action name=\"ApplyContentMarking\"> <argument key=\"Text\" value=\"Classified as Contoso Confidential\" /> <argument key=\"FontSize\" value=\"10\" /> <argument key=\"FontColor\" value=\"#000000\" /> <argument key=\"Alignment\" value=\"Left\" /> <argument key=\"Placement\" value=\"Footer\" /> <argument key=\"Margin\" vaalue=\"5\" /> </action> </rule> <rule name=\"ConfidentialRecipientsOnly\" id=\"91b1792b-d5c6-4a43-813a-576aa3d9900f\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"c8deb7ae-676d-41d3-b0b3-51ee9c4d9b7b\" /> </keyValues> </containsClassification> </condition> <action name=\"RightsProtectMessage\"> <argument key=\"ProtectionType\" value=\"DoNotForward\" /> </action> </rule> <rule name=\"ConfidentialMSExecAndStaff\" id=\"7b0b1e3a-60d8-4c4f-bd72-cc830073d622\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"0d23bc70-d65e-429c-99d4-a2176d198741\" /> </keyValues> </containsClassification> </condition> <action name=\"RightsProtectMessage\"> <argument key=\"ProtectionType\" value=\"Template\" /> <argument key=\"TemplateId\" value=\"{2be80f00-49a2-448f-b0d3-e3b52f37aea1}\" /> </action> </rule> <rule name=\"ConfidentialContosoFTE\" id=\"f2bfde0d-719b-4b06-a6c0-b9d119f0815b\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"7de135f8-ae65-4421-b996-b9c2b8aae5a7\" /> </keyValues> </containsClassification> </condition> <action name=\"RightsProtectMessage\"> <argument key=\"ProtectionType\" value=\"Template\" /> <argument key=\"TemplateId\" value=\"{2c64e36e-c9e3-439e-8268-644fa55a92d8}\" /> </action> </rule> <rule name=\"ConfidentialContosoExtended\" id=\"1d75602c-26d0-448d-b448-b2ef500f3402\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"bd621434-11b6-4854-84a0-5e9df216fb7c\" /> </keyValues> </containsClassification> </condition> <action name=\"RightsProtectMessage\"> <argument key=\"ProtectionType\" value=\"Template\" /> <argument key=\"TemplateId\" value=\"{cb92b4db-d289-4664-8120-c5d09cbe4130}\" /> </action> </rule> <rule name=\"ConfidentialAnyUserNoProtection\" id=\"0b2610db-29a2-43a6-b4a0-fd8834f67842\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"5d7759ff-504a-433f-8023-8780e280f4fb\" /> </keyValues> </containsClassification> </condition> <action name=\"RemoveProtection\" /> </rule> <rule name=\"ConfidentialIPRMSTeamTestLabel\" id=\"0b2610db-29a2-43a6-b4a0-fd8834f67842\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"a3d4a808-51c7-4b03-9736-2c163a738577\" /> </keyValues> </containsClassification> </condition> <action name=\"RightsProtectMessage\"> <argument key=\"ProtectionType\" value=\"Template\" /> <argument key=\"TemplateId\" value=\"{cb92b4db-d289-4664-8120-c5d09cbe4130}\" /> </action> </rule> <rule name=\"HighlyConfidential\" id=\"0b8461e0-5777-43e8-a715-cd519f15f389\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"98c836a2-1532-4e89-877b-7dc674798756\" /> </keyValues> </containsClassification> </condition> <action name=\"ApplyContentMarking\"> <argument key=\"Text\" value=\"Classified as Contoso Highly Confidential\" /> <argument key=\"FontSize\" value=\"10\" /> <argument key=\"FontColor\" value=\"#000000\" /> <argument key=\"Alignment\" value=\"Left\" /> <argument key=\"Placement\" value=\"Header\" /> <argument key=\"Margin\" value=\"5\" /> </action> </rule> <rule name=\"HighlyConfidentialRecipientsOnly\" id=\"f547fc4c-622a-40f5-b0a9-3f879d49260a\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"21674434-b332-4647-948b-7882b4a55807\" /> </keyValues> </containsClassification> </condition> <action name=\"RightsProtectMessage\"> <argument key=\"ProtectionType\" value=\"DoNotForward\" /> </action> </rule> <rule name=\"HighlyConfidentialMSExecutivesAndStaff\" id=\"a3375b85-a717-487c-82b3-cbbc2243e3c7\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"8ef3c83a-d6c2-4a49-aa37-ae5ef3739ce6\" /> </keyValues> </containsClassification> </condition> <action name=\"RightsProtectMessage\"> <argument key=\"ProtectionType\" value=\"Template\" /> <argument key=\"TemplateId\" value=\"{20616d19-48f6-4011-bf77-523161e4eccc}\" /> </action> </rule> <rule name=\"HighlyConfidentialContosoFTE\" id=\"b4be4a2b-25f4-48e7-947a-cc97681eb8c8\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"5fb6eaa5-56a9-4aeb-8719-f45f81471183\" /> </keyValues> </containsClassification> </condition> <action name=\"RightsProtectMessage\"> <argument key=\"ProtectionType\" value=\"Template\" /> <argument key=\"TemplateId\" value=\"5fd0e23c-dfcd-404f-9db3-9196e3aea280\" /> </action> </rule> <rule name=\"HighlyConfidentialContosoExtended\" id=\"b4be4a2b-25f4-48e7-947a-cc97681eb8c8\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"d83c8aa3-afac-4478-9759-46608805e2bd\" /> </keyValues> </containsClassification> </condition> <action name=\"RightsProtectMessage\"> <argument key=\"ProtectionType\" value=\"Template\" /> <argument key=\"TemplateId\" value=\"{6830ea82-aa19-42ba-a6b1-fe3af8e80e4a}\" /> </action> </rule> <rule name=\"HighlyConfidentialAnyUserNoProtection\" id=\"b4be4a2b-25f4-48e7-947a-cc97681eb8c8\"> <condition> <containsClassification property=\"Item.ClassificationDiscovered\"> <keyValues> <keyValue key=\"Label\" value=\"152d0062-7ac3-413c-9447-a217e8eb05b3\" /> </keyValues> </containsClassification> </condition> <action name=\"RemoveProtection\" /> </rule> </rules> </policy> </policies> </Content> </SyncFile>";
}

class ProfileObserverImpl : public IProfile::Observer {
    // lock on the cosntrucot of this class
public:
    virtual void OnLoadSuccess(
                               shared_ptr<IProfile> profile,
                               shared_ptr<void> context) {
        cout << "Load Success ..." << endl;
        // 1) Uncomment this and comment the rest to load the stub implementation from the binaries
        /* mip_profile = profile;
         processed = true;
         cv.notify_one();*/
        
        // 2) Chose your own xml
        
        mip_profile = profile;
        mip::IPolicyEngine::Settings engine_settings("1", "", nullptr);
        vector<pair<string, string>> cs;
        // Either specify the policy file path as:
        cs.push_back(make_pair("policy_file", "PolicyWithLabels.xml"));
        // OR speicfy XML in hard-coded string as:
        //cs.push_back(make_pair("policy_data", someXml));
        engine_settings.SetCustomSettings(cs);
        profile->AddEngineAsync(engine_settings, nullptr);
        cv.notify_one();
    }
    virtual void OnLoadFailure(const IMipError& error, shared_ptr<void> context) override {
        cout << "Load Failed ..." << endl;
    }
    
    virtual void OnListEnginesSuccess(
                                      std::vector<string> engineIds,
                                      shared_ptr<void> context) override {
        cout << "List Engine Success ..." << endl;
        processed = true;
        cv.notify_one();
    }
    virtual void OnListEnginesError(
                                    const IMipError& error,
                                    shared_ptr<void> context) override {
        cout << "List Engine Error ..." << endl;
        cv.notify_one();
    }
    
    virtual void OnLoadEngineSuccess(
                                     shared_ptr<IPolicyEngine> engine,
                                     shared_ptr<void> context) override {
        cout << "Load Engine Success ..." << endl;
        mip_engine = engine;
        processed = true;
        cv.notify_one();
    }
    virtual void OnLoadEngineError(const IMipError& error, shared_ptr<void> context) override {
        cout << "Load Engine Error ..." << endl;
        cv.notify_one();
    }
    
    virtual void OnUnloadEngineSuccess(shared_ptr<void> context) override {
        cout << "Unload Engine Success ..." << endl;
        processed = true;
        cv.notify_one();
    }
    virtual void OnUnloadEngineError(const IMipError& error, shared_ptr<void> context) override {
        cout << "Unload Engine Error ..." << endl;
        cv.notify_one();
    }
    
    virtual void OnAddEngineSuccess(
                                    shared_ptr<IPolicyEngine> engine,
                                    shared_ptr<void> context) override {
        cout << "Add Engine Success ..." << endl;
        std::vector < shared_ptr<ILabel>> labels = engine->ListSensitivtyLabels();
        mip_engine = engine;
        processed = true;
        cv.notify_one();
    }
    virtual void OnAddEngineError(const IMipError& error, shared_ptr<void> context) override {
        cout << "Add Engine Error ..." << endl;
        cv.notify_one();
    }
    
    virtual void OnDeleteEngineSuccess(shared_ptr<void> context) override {
        cout << "Delete Engine Success ..." << endl;
        cv.notify_one();
    }
    virtual void OnDeleteEngineError(const IMipError& error, shared_ptr<void> context) override {
        cout << "Delete Engine Error ..." << endl;
        cv.notify_one();
    }
};

class myObserver : public IPolicyEngine::Observer {
    void OnPolicyChanged(const string& engineId) {
        cout << "Changing policy " << engineId << std::endl;
    }
};

class AuthDelegateImpl : public IAuthDelegate {
public:
    AuthDelegateImpl(std::string token) : mToken(token) {};
    ~AuthDelegateImpl() override {};
    bool AcquireOAuth2Token(const std::string&,
                            const OAuth2Challenge& challenge,
                            OAuth2Token& token)
    {
        token = mToken;
        return true;
    }
private:
    std::string mToken;
};

int main()
{
    shared_ptr<ProfileObserverImpl> observer(new ProfileObserverImpl());
    cout << "Loading Profile..." << endl;
    shared_ptr<IAuthDelegate> auth(new AuthDelegateImpl(""));
    IProfile::Settings settings("xyz", auth, observer);
    IProfile::LoadAsync(settings, nullptr);
    
    // wait for thread
    {
        std::unique_lock<mutex> lk(mtx);
        cv.wait(lk, [] {return processed; });
    }
    //processed = false;
    //int choice;
    // cout << "Adding an Engine" << endl;
    // string client_data = "This is client data";
    // string engineId = "000001";
    //IPolicyEngine::Settings engine_settings(engineId, client_data, nullptr);
    //mip_profile->AddEngineAsync(engine_settings, nullptr);
    
    //// wait for thread
    //{
    //	std::unique_lock<mutex> lk(mtx);
    //	cv.wait(lk, [] {return processed; });
    //}
    
    std::vector < shared_ptr<ILabel>> labels = mip_engine->ListSensitivtyLabels();
    for (size_t i = 0; i < labels.size(); i++) {
        std::cout << "[" << i << "]  ID: " << labels[i]->GetId() << " Name: " << labels[i]->GetName() << endl;
    }
    //Ask which label they want
    int index;
    do {
        cout << "Select Label to extend [0-" << labels.size() - 1 << "] : " << endl;
        cin >> index;
    } while (index > labels.size() || index < 0);
    
    if (labels[index]->GetChildren().size() > 0) {
        int childIndex = -1;
        std::vector < shared_ptr<ILabel>> childLabels = labels[index]->GetChildren();
        for (size_t i = 0; i < childLabels.size(); i++) {
            std::cout << "[" << i << "]  ID: " << childLabels[i]->GetId() << " Name: " << childLabels[i]->GetName() << endl;
        }
        do {
            cout << "Select Label to extend [0-" << childLabels.size() - 1 << "] : " << endl;
            cin >> childIndex;
        } while (childIndex > childLabels.size() || childIndex < 0);
        
        // if there is a child label, overwrite nextLabelId to the child label
        nextLabelId = childLabels[childIndex]->GetId();
    }
    else {
        nextLabelId = labels[index]->GetId();
        cout << labels[index]->GetName() << " has no children." << endl;
    }
    
    ExecutionState ex_state;
    
    // Computes the actions that have to be executed in order for nextLabelId to be applied
    vector<shared_ptr<IAction>> actions = mip_engine->ComputeActions(ex_state);
    for (size_t i = 0; i < actions.size(); i++) {
        std::cout << "[" << i << "]  ID: " << actions[i]->GetId() << " Type: ";
        switch (actions[i]->GetType()) {
            case IAction::ActionType::ACTION_TYPE_PROTECT_ADHOC:
                cout << "Protect Adhoc Action";
                break;
            case IAction::ActionType::ACTION_TYPE_PROTECT_BY_TEMPLATE:
                cout << "Protect by Template Action";
                break;
            case IAction::ActionType::ACTION_TYPE_PROTECT_DO_NOT_FORWARD:
                cout << "Do Not Forward Action";
                break;
            case IAction::ActionType::ACTION_TYPE_METADATA:
                cout << "Metadata Action";
                break;
            case IAction::ActionType::ACTION_TYPE_CONTENT_MARKING:
                cout << "Content Marking Action";
                break;
            case IAction::ActionType::ACTION_TYPE_JUSTIFY:
                cout << "Justify Action";
                break;
            case IAction::ActionType::ACTION_TYPE_WATER_MARKING:
                cout << "Water Marking Action";
                break;
            default:
                break;
        }
        cout << endl;
    }
    return 0;
}

