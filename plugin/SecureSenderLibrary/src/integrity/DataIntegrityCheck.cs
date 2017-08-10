using System.Collections.Generic;
using Newtonsoft.Json;
using rmscore.wrapper;
using SecureSenderLibrary.core;
using SecureSenderLibrary.models;
using SecureSenderLibrary.models.json;

namespace SecureSenderLibrary.integrity
{
    /// <summary>
    /// Checks if a list of IProperty objects matches up with a DataCollection
    /// </summary>
    /// <typeparam name="T">Implementation of IProperty</typeparam>
    public class DataIntegrityCheck<T> where T : IProperty
    {
        private readonly DataCollectionExp _dataCollection;
        private readonly IHasher _dataHasher;
        /// <summary>
        /// The Integrity Status of the object (see enum for details)
        /// </summary>
        public IntegrityStatus Status { get; }
        /// <summary>
        /// The properties inside of the list which match up with the DataCollection
        /// </summary>
        public IList<T> VerifiedProperties { get; }
        /// <summary>
        /// The properties inside the list which didn't match up with the DataCollection
        /// </summary>
        public IList<T> UnverifiedProperties { get; }
        public CertificateJsonObject Certificate { get; }

        public DataIntegrityCheck(string dataCollectionJson, IEnumerable<T> unverifiedData)
        {
            Status = IntegrityStatus.Verified;
            if (dataCollectionJson == null)
            {
                Status = IntegrityStatus.Missing;
                return;
            }
            _dataCollection = new DataCollectionExp(dataCollectionJson);
            if(_dataCollection.ContainsKey(Resources.hash_algorithm_key))
                _dataHasher = new HasherManager().GetHasherForAlgorithm(_dataCollection.GetDataValue(Resources.hash_algorithm_key));
            if (_dataHasher == null)
                Status = IntegrityStatus.Malformed;
            else if(!_dataCollection.IsVerified())
                Status = IntegrityStatus.FailedServerValidation;
            else
            {
                VerifiedProperties = new List<T>();
                UnverifiedProperties = new List<T>();
                foreach (var property in unverifiedData)
                {
                    if (!CheckPropertyAgainstData(property))
                        UnverifiedProperties.Add(property);
                    else
                        VerifiedProperties.Add(property);
                }
                var certificateJson = _dataCollection.GetCertificateJson();
                certificateJson = certificateJson.Replace(@"\", " ");
                Certificate = JsonConvert.DeserializeObject<CertificateJsonObject>(certificateJson);
                if (UnverifiedProperties.Count > 0)
                    Status = IntegrityStatus.DataMismatch;
            }
        }

        private bool CheckPropertyAgainstData(T property)
        {
            if (!_dataCollection.ContainsKey(property.Name))
                return true;
            var dataCollectionValue = _dataCollection.GetDataValue(property.Name);
            var unverifiedDataValue = property.Value;
            return dataCollectionValue.Equals(_dataHasher.Hash(unverifiedDataValue));
        }
    }
}
