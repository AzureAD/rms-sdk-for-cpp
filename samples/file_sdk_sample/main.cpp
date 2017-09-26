#include "main.h"
#include <iostream>
#include <memory>
#include <fstream>

#include "cxxopts/cxxopts.hpp"
#include <Common/file_format_factory.h>
#include <Common/std_stream_adapter.h>
#include <api/istream_handler.h>
#include <Common/tag.h>
#include <CryptoAPI.h>
#include "utils.h"

using std::cout;
using mip::file::StdStreamAdapter;
using mip::Tag;

namespace {
Tag kGeneral("{434fea85-168e-4781-8780-6f89f3887290}", "General", "", "Assaf", true, "now", mip::Method::NONE);
Tag kPersonal("{23a793b5-66c0-4dec-adb6-7928bb6df345}", "Personal", "", "Assaf", true, "now", mip::Method::MANUAL);
Tag kSecret("{ccf69ce5-ac74-43e9-ba46-eb73ed3cac1b}", "Secret", "", "Assaf", true, "now", mip::Method::MANUAL);
std::vector<Tag> kLabels { kGeneral, kPersonal, kSecret};
}

int main(int argc, char *argv[]) {
  try {
    cxxopts::Options options(argv[0], "Sample app for Microsoft Information Protection SDK");
    options.positional_help("<Extra args>");

    options.add_options()
        ("f,file", "Full path to the file to work on.", cxxopts::value<std::string>(), "File path")
        ("g,get-label", "Show the labels that applies on a file.")
        ("s,set-label", "Set a label with <labelId> and with owner <owner>. "
                        "If downgrading label - will apply <Justification message> as needed.", cxxopts::value<std::vector<std::string>>(), "lblId, owner, msg")
        ("r,remove-label", "Remove the current label from the file with <Justification message>.", cxxopts::value<std::string>()->implicit_value("User Justification"), "msg")
        ("l,list-labels", "Show all available labels with there Id's.")
        ("h,help", "Print help and exit.")
        ;

    options.parse_positional({"file", "remove-label", "set-label"});
    options.parse(argc, argv);

    // help
    if (options.count("help")){
      cout << options.help({""}) << std::endl;
      exit(0);
    }

    // list-labels
    if (options.count("list-labels")) {
      ListLabels();
      exit(0);
    }

    // file
    std::string filePath;
    if (options.count("file")) {
      filePath = options["file"].as<std::string>();
    }

    if (filePath.empty()) {
      cout << options.help({""}) << std::endl;
      exit(0);
    }

    // get-label
    if (options.count("get-label")) {
      GetLabels(filePath);
      exit(0);
    }

    // set-label
    if (options.count("set-label")) {
      std::vector<std::string> additionalArgs = options["set-label"].as<std::vector<std::string>>();
      std::string labelId(additionalArgs[0]);
      std::string owner = "", justificationMessage = "";
      if (additionalArgs.size() == 2) {
        owner = additionalArgs[1];
      }
      else if (additionalArgs.size() == 3) {
        owner = additionalArgs[1];
        justificationMessage = additionalArgs[2];
      }

      SetLabels(filePath, labelId, owner, justificationMessage);
      exit(0);
    }

    // remove-label
    if (options.count("remove-label")) {
      std::string justificationMessage = options["remove-label"].as<std::string>();

      // SetLabel without labelId delete the label
      SetLabels(filePath, "", "", justificationMessage);
      exit(0);
    }

    // default when there is a only file path - Show labels
    GetLabels(filePath);

  } catch (const cxxopts::OptionException& e) {
    cout << "error parsing options: " << e.what() << std::endl;
    exit(-1);
  }

  return 0;
}

void GetLabels(std::string filePath) {
  auto inputFileStream = std::make_shared<std::ifstream>(filePath, std::ios::binary);
  auto stream = StdStreamAdapter::Create(inputFileStream);
  std::string extension = GetExtenstion(filePath);

  auto streamHandler = mip::file::IStreamHandler::Create(nullptr, stream, extension);

  try {
    if (!streamHandler->IsLabeled()) {
      cout << "File is not labeled" << std::endl;
      exit(0);
    }

    auto tag = streamHandler->GetLabel();
    cout << "File is labeled as: " << tag->GetLabelName() << "\nId: " << tag->GetLabelId() << "\nOwner: " << tag->GetOwner() <<std::endl;

  } catch (std::runtime_error) {
    cout << "Can't parse file extension" << std::endl;
  } catch (...) {
    cout << "Something bad happend. problaly not implemented yet" << std::endl;
  }
}

std::string GetExtenstion(const std::string& filePath) {
  std::string::size_type idx;

  idx = filePath.rfind('.');

  if(idx != std::string::npos) {
    return filePath.substr(idx); // to include the dot in the file extention
  }
  else {
    throw std::runtime_error("not implemented");
  }
}

void SetLabels(std::string filePath, std::string labelId, std::string owner, std::string justificationMessage) {
  try {
    std::string extension = GetExtenstion(filePath);
    LabelingOptions labelingOptions(justificationMessage, mip::Method::MANUAL, owner);

    // Create input stream
    auto iFileStream = std::make_shared<std::ifstream>(filePath, std::ios::binary);
    auto inputStream = StdStreamAdapter::Create(std::static_pointer_cast<std::istream>(iFileStream));
    auto streamHandler = mip::file::IStreamHandler::Create(nullptr, inputStream, extension);

    // Get the new file path
    std::string newFilePath = filePath.substr(0, filePath.length() - extension.length()) + (labelId.empty() ? "_NotLabeled" : "_Labeled");

    // Create output stream
    auto outfile = std::make_shared<std::fstream>(newFilePath + extension, std::ios::binary| std::ios::in | std::ios::out | std::ios::trunc);
    auto outputStream = StdStreamAdapter::Create(std::static_pointer_cast<std::iostream>(outfile));

    if (labelId.empty()) {
      streamHandler->DeleteLabel(outputStream, justificationMessage);
    }
    else {
      streamHandler->SetLabel(outputStream, labelId, labelingOptions);
    }
  } catch (std::runtime_error ex) {
    cout << "Something bad happend. " << ex.what() << std::endl;
    exit(-1);
  } catch (std::exception ex) {
    cout << "Error: " << ex.what() << std::endl;
    exit(-1);
  }
}

void ListLabels() {
  for (const Tag& label: kLabels) {
    cout << label.GetLabelName() << " " << label.GetLabelId() << std::endl;
  }
}
