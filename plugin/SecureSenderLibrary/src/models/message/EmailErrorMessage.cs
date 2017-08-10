namespace SecureSenderLibrary.models.message
{
    public class EmailErrorMessage : InformationMessage
    {
        public EmailErrorMessage(IntegrityStatus emailStatus)
        {
            Name = Resources.error_title;
            switch (emailStatus)
            {
                case IntegrityStatus.DataMismatch:
                    Details = Resources.doesnt_match_email_error_detail;
                    break;
                case IntegrityStatus.FailedServerValidation:
                    Details = Resources.failed_server_validation_error;
                    break;
                case IntegrityStatus.Malformed:
                    Details = Resources.malformed_error;
                    break;
                default:
                    Details = Resources.unspecified_error;
                    break;
            }
        }
    }
}
