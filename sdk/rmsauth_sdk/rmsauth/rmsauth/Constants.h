/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef CONSTANTS
#define CONSTANTS
#include "types.h"

namespace rmsauth {

struct Constants
{
    struct RmsauthError {
        /// <summary>
        /// Unknown error.
        /// </summary>
        const String Unknown = "unknown_error";

        /// <summary>
        /// Invalid argument.
        /// </summary>
        const String InvalidArgument = "invalid_argument";

        /// <summary>
        /// Authentication failed.
        /// </summary>
        const String AuthenticationFailed = "authentication_failed";

        /// <summary>
        /// Authentication canceled.
        /// </summary>
        const String AuthenticationCanceled = "authentication_canceled";

        /// <summary>
        /// Unauthorized response expected from resource server.
        /// </summary>
        const String UnauthorizedResponseExpected = "unauthorized_response_expected";

        /// <summary>
        /// 'authority' is not in the list of valid addresses.
        /// </summary>
        const String AuthorityNotInValidList = "authority_not_in_valid_list";

        /// <summary>
        /// Authority validation failed.
        /// </summary>
        const String AuthorityValidationFailed = "authority_validation_failed";

        /// <summary>
        /// Loading required assembly failed.
        /// </summary>
        const String AssemblyLoadFailed = "assembly_load_failed";

        /// <summary>
        /// Loading required assembly failed.
        /// </summary>
        const String InvalidOwnerWindowType = "invalid_owner_window_type";

        /// <summary>
        /// MultipleTokensMatched were matched.
        /// </summary>
        const String MultipleTokensMatched = "multiple_matching_tokens_detected";

        /// <summary>
        /// Invalid authority type.
        /// </summary>
        const String InvalidAuthorityType = "invalid_authority_type";

        /// <summary>
        /// Invalid credential type.
        /// </summary>
        const String InvalidCredentialType = "invalid_credential_type";

        /// <summary>
        /// Invalid service URL.
        /// </summary>
        const String InvalidServiceUrl = "invalid_service_url";

        /// <summary>
        /// failed_to_acquire_token_silently.
        /// </summary>
        const String FailedToAcquireTokenSilently = "failed_to_acquire_token_silently";

        /// <summary>
        /// Certificate key size too small.
        /// </summary>
        const String CertificateKeySizeTooSmall = "certificate_key_size_too_small";

        /// <summary>
        /// Identity protocol login URL Null.
        /// </summary>
        const String IdentityProtocolLoginUrlNull = "identity_protocol_login_url_null";

        /// <summary>
        /// Identity protocol mismatch.
        /// </summary>
        const String IdentityProtocolMismatch = "identity_protocol_mismatch";

        /// <summary>
        /// Email address suffix mismatch.
        /// </summary>
        const String EmailAddressSuffixMismatch = "email_address_suffix_mismatch";

        /// <summary>
        /// Identity provider request failed.
        /// </summary>
        const String IdentityProviderRequestFailed = "identity_provider_request_failed";

        /// <summary>
        /// STS token request failed.
        /// </summary>
        const String StsTokenRequestFailed = "sts_token_request_failed";

        /// <summary>
        /// Encoded token too long.
        /// </summary>
        const String EncodedTokenTooLong = "encoded_token_too_long";

        /// <summary>
        /// Service unavailable.
        /// </summary>
        const String ServiceUnavailable = "service_unavailable";

        /// <summary>
        /// Service returned error.
        /// </summary>
        const String ServiceReturnedError = "service_returned_error";

        /// <summary>
        /// Federated service returned error.
        /// </summary>
        const String FederatedServiceReturnedError = "federated_service_returned_error";

        /// <summary>
        /// STS metadata request failed.
        /// </summary>
        const String StsMetadataRequestFailed = "sts_metadata_request_failed";

        /// <summary>
        /// No data from STS.
        /// </summary>
        const String NoDataFromSts = "no_data_from_sts";

        /// <summary>
        /// User Mismatch.
        /// </summary>
        const String UserMismatch = "user_mismatch";

        /// <summary>
        /// Unknown User Type.
        /// </summary>
        const String UnknownUserType = "unknown_user_type";

        /// <summary>
        /// Unknown User.
        /// </summary>
        const String UnknownUser = "unknown_user";

        /// <summary>
        /// User Realm Discovery Failed.
        /// </summary>
        const String UserRealmDiscoveryFailed = "user_realm_discovery_failed";

        /// <summary>
        /// Accessing WS Metadata Exchange Failed.
        /// </summary>
        const String AccessingWsMetadataExchangeFailed = "accessing_ws_metadata_exchange_failed";

        /// <summary>
        /// Parsing WS Metadata Exchange Failed.
        /// </summary>
        const String ParsingWsMetadataExchangeFailed = "parsing_ws_metadata_exchange_failed";

        /// <summary>
        /// WS-Trust Endpoint Not Found in Metadata Document.
        /// </summary>
        const String WsTrustEndpointNotFoundInMetadataDocument = "wstrust_endpoint_not_found";

        /// <summary>
        /// Parsing WS-Trust Response Failed.
        /// </summary>
        const String ParsingWsTrustResponseFailed = "parsing_wstrust_response_failed";

        /// <summary>
        /// The request could not be preformed because the network is down.
        /// </summary>
        const String NetworkNotAvailable = "network_not_available";

        /// <summary>
        /// The request could not be preformed because of an unknown failure in the UI flow.
        /// </summary>
        const String AuthenticationUiFailed = "authentication_ui_failed";

        /// <summary>
        /// One of two conditions was encountered.
        /// 1. The PromptBehavior.Never flag was passed and but the constraint could not be honored
        ///    because user interaction was required.
        /// 2. An error occurred during a silent web authentication that prevented the authentication
        ///    flow from completing in a short enough time frame.
        /// </summary>
        const String UserInteractionRequired = "user_interaction_required";

        /// <summary>
        /// Password is required for managed user.
        /// </summary>
        const String PasswordRequiredForManagedUserError = "password_required_for_managed_user";

        /// <summary>
        /// Failed to get user name.
        /// </summary>
        const String GetUserNameFailed = "get_user_name_failed";

        /// <summary>
        /// Federation Metadata Url is missing for federated user.
        /// </summary>
        const String MissingFederationMetadataUrl = "missing_federation_metadata_url";

        /// <summary>
        /// Failed to refresh token.
        /// </summary>
        const String FailedToRefreshToken = "failed_to_refresh_token";

        /// <summary>
        /// Integrated authentication failed. You may try an alternative authentication method.
        /// </summary>
        const String IntegratedAuthFailed = "integrated_authentication_failed";

        /// <summary>
        /// Duplicate query parameter in extraQueryParameters
        /// </summary>
        const String DuplicateQueryParameter = "duplicate_query_parameter";
    };

    static const RmsauthError& rmsauthError()
    {
        static const RmsauthError rmsauthError{};
        return rmsauthError;
    }

    struct RmsauthErrorMessage{
        const String AccessingMetadataDocumentFailed = "Accessing WS metadata exchange failed";
        const String AssemblyLoadFailedTemplate = "Loading an assembly required for interactive user authentication failed. Make sure assembly '%' exists";
        const String AuthenticationUiFailed = "The browser based authentication dialog failed to complete";
        const String AuthorityInvalidUriFormat = "'authority' should be in Uri format";
        const String AuthorityNotInValidList = "'authority' is not in the list of valid addresses";
        const String AuthorityValidationFailed = "Authority validation failed";
        const String AuthorityUriInsecure = "'authority' should use the 'https' scheme";
        const String AuthorityUriInvalidPath = "'authority' Uri should have at least one segment in the path (i.e. https://<host>/<path>/...)";
        const String AuthorizationServerInvalidResponse = "The authorization server returned an invalid response";
        const String CertificateKeySizeTooSmallTemplate = "The certificate used must have a key size of at least % bits";
        const String EmailAddressSuffixMismatch = "No identity provider email address suffix matches the provided address";
        const String EncodedTokenTooLong = "Encoded token size is beyond the upper limit";
        const String FailedToAcquireTokenSilently = "Failed to acquire token silently. Call method AcquireToken";
        const String FailedToRefreshToken = "Failed to refresh token";
        const String FederatedServiceReturnedErrorTemplate = "Federated serviced at % returned error: %";
        const String IdentityProtocolLoginUrlNull = "The LoginUrl property in identityProvider cannot be null";
        const String IdentityProtocolMismatch = "No identity provider matches the requested protocol";
        const String IdentityProviderRequestFailed = "Token request to identity provider failed. Check InnerException for more details";
        const String InvalidArgumentLength = "Parameter has invalid length";
        const String InvalidAuthenticateHeaderFormat = "Invalid authenticate header format";
        const String InvalidAuthorityTypeTemplate = "This method overload is not supported by ";
        const String InvalidCredentialType = "Invalid credential type";
        const String InvalidFormatParameterTemplate = "Parameter '%' has invalid format";
        const String InvalidTokenCacheKeyFormat = "Invalid token cache key format";
        const String MissingAuthenticateHeader = "WWW-Authenticate header was expected in the response";
        const String MultipleTokensMatched = "The cache contains multiple tokens satisfying the requirements. Call AcquireToken again providing more requirements (e.g. UserId)";
        const String NetworkIsNotAvailable = "The network is down so authentication cannot proceed";
        const String NoDataFromSTS = "No data received from security token service";
        const String NullParameterTemplate = "Parameter '%' cannot be null";
        const String ParsingMetadataDocumentFailed = "Parsing WS metadata exchange failed";
        const String ParsingWsTrustResponseFailed = "Parsing WS-Trust response failed";
        const String PasswordRequiredForManagedUserError = "Password is required for managed user";
        const String RedirectUriContainsFragment = "'redirectUri' must NOT include a fragment component";
        const String ServiceReturnedError = "Serviced returned error. Check InnerException for more details";
        const String StsMetadataRequestFailed = "Metadata request to Access Control service failed. Check InnerException for more details";
        const String StsTokenRequestFailed = "Token request to security token service failed.  Check InnerException for more details";
        const String UnauthorizedHttpStatusCodeExpected = "Unauthorized Http Status Code (401) was expected in the response";
        const String UnauthorizedResponseExpected = "Unauthorized http response (status code 401) was expected";
        const String UnexpectedAuthorityValidList = "Unexpected list of valid addresses";
        const String Unknown = "Unknown error";
        const String UnknownUser = "Could not identify logged in user";
        const String UnknownUserType = "Unknown User Type";
        const String UnsupportedAuthorityValidation = "Authority validation is not supported for this type of authority";
        const String UnsupportedMultiRefreshToken = "This authority does not support refresh token for multiple resources. Pass null as a resource";
        const String AuthenticationCanceled = "User canceled authentication";
        const String UserMismatch = "User '%' returned by service does not match user '%' in the request";
        const String UserCredentialAssertionTypeEmpty = "credential.AssertionType cannot be empty";
        const String UserInteractionRequired =
            "One of two conditions was encountered: "\
            "1. The PromptBehavior.Never flag was passed, but the constraint could not be honored, because user interaction was required. "\
            "2. An error occurred during a silent web authentication that prevented the http authentication flow from completing in a short enough time frame";
        const String UserRealmDiscoveryFailed = "User realm discovery failed";
        const String WsTrustEndpointNotFoundInMetadataDocument = "WS-Trust endpoint not found in metadata document";
        const String GetUserNameFailed = "Failed to get user name";
        const String MissingFederationMetadataUrl = "Federation Metadata Url is missing for federated user. This user type is unsupported.";
        const String SpecifyAnyUser = "If you do not need access token for any specific user, pass userId=UserIdentifier::anyUser() instead of userId=null.";
        const String IntegratedAuthFailed = "Integrated authentication failed. You may try an alternative authentication method";
        const String DuplicateQueryParameterTemplate = "Duplicate query parameter '%' in extraQueryParameters";
    };

    static const RmsauthErrorMessage& rmsauthErrorMessage()
    {
        static const RmsauthErrorMessage rmsauthErrorMessage{};
        return rmsauthErrorMessage;
    }

    struct  RmsauthIdParameter {
        /// <summary>
        /// RMSAUTH Flavor
        /// </summary>
        const String Product = "x-client-SKU";
        const String ProductVal = "RMSAuth";

        /// <summary>
        /// RMSAUTH assembly version
        /// </summary>
        const String Version = "x-client-Ver";
        const String VersionVal = "0.1";

        /// <summary>
        /// CPU platform with x86, x64 or ARM as value
        /// </summary>
        const String CpuPlatform = "x-client-CPU";

        /// <summary>
        /// Version of the operating system. This will not be sent on WinRT
        /// </summary>
        const String OS = "x-client-OS";

        /// <summary>
        /// Device model. This will not be sent on .NET
        /// </summary>
        const String UserAgent = "x-client-DM";

        const String DeviceModel = "RMSAuth";
    };

    static const RmsauthIdParameter& rmsauthIdParameter()
    {
        static const RmsauthIdParameter rmsauthIdParameter{};
        return rmsauthIdParameter;
    }
};

} // namespace rmsauth {

#endif // CONSTANTS

