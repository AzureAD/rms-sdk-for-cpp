using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using rmscore.wrapper;

namespace DotNetRmsWrapperTests
{
    [TestClass]
    public class DataCollectionExpTests
    {
        [TestMethod]
        public void TestConstructorEmptyJSON()
        {
            DataCollectionExp dcExp = new DataCollectionExp("");
            Assert.IsFalse(dcExp.IsVerified());
        }

        [TestMethod]
        public void TestConstructorValidJSON()
        {
            String json = @"{""data"":{""Key"":""Value""},""signature"":{""algorithm"":"""",""certificate"":{""pld"":{""pub"":{""pld"":{""iss"":{""eurl"":""https://48fc04bd-c84b-44ac-91b7-a4c5eefd5ac1.rms.na.aadrm.com/_wmcs/certification"",""fname"":""CorpRights"",""id"":""{aaa331f3-e659-4ea9-876b-e8b25c7b4996}"",""iurl"":""https://48fc04bd-c84b-44ac-91b7-a4c5eefd5ac1.rms.na.aadrm.com/_wmcs/certification"",""pubk"":{""e"":""65537"",""kty"":""RSA"",""n"":""oxyxbUguymFHLhu80EH9m62c2XLFXbjm87KlEbSFQkieurr8RZgvtEMWdg6jMAkLJ7xPnWs3hEjOHw99irwZ1g5Z+6yYgktWlKibmPs5Lpamd6v53R40fKES/HNWdjbc68xN9tyfts4pOfyJxT5Xgo0rHVset+rDgShE9B3JQbY=""}},""issto"":{""em"":""t-joanto@microsoft.com"",""id"":{""typ"":""Federation"",""val"":""62012187-8d83-4d8a-b996-6af37b0f1d54""},""pubk"":{""e"":""65537"",""kty"":""RSA"",""n"":""ZeLjkvbnkVOUoeso21kZUcWhnDTh/ddwrp0+NKj8aqfyEHjfp2Jc38fW9LbWhqPBWjrad9cBdKkk7RHxFYZwIQXbDAPa18W6FvfAL67Bq9ETZ+0ugO2Gb66SYXH7VFg3bQHPF37l0sVaMKluxv3nY1ZDnlLDsMidHAwyzCdh1MU=""}}},""sig"":{""alg"":""SHA1"",""dig"":""CELn+qiDYKrSREHAdu5EQluTgKcjOGgDo7GtoQIJihCc9F3CUASUFnEWYfCRPoghWKZ2/hCWENAJWVnNDqhy1RHlWt+jl1S4I1CdHPSRj8Rp5IbiEAoXHAviHrns47OTLq5dWPdQ8BN4DtfSSTqNBoTI3QdFQW8d8C55GxjYjz4="",""penc"":""utf-8""}}}},""signed_data_hash"":""j/WbpShglfBevSmCi8ijdsqlpnd20wthAJgjBtFM2Zz8G1cf7svdyPHF3Aq6CAeVBqVpOHePCR/LVcEjnPy0qOetuCnDblfTRhQIGt1iUoODVg0nHMV7gfwQChyQnzChu53kv9VNYANpna3YbTwok2af09EHjhoyWxSh0BzLnRM=""}}";
            DataCollectionExp dcExp = new DataCollectionExp(json);
            Assert.IsTrue(dcExp.IsVerified());
        }

        [TestMethod]
        public void TestConstructorInvalidJSON()
        {
            String json = @"{""data"":{""Key"":""Vaelue""},""signature"":{""algorithm"":"""",""certificate"":{""pld"":{""pub"":{""pld"":{""iss"":{""eurl"":""https://48fc04bd-c84b-44ac-91b7-a4c5eefd5ac1.rms.na.aadrm.com/_wmcs/certification"",""fname"":""CorpRights"",""id"":""{aaa331f3-e659-4ea9-876b-e8b25c7b4996}"",""iurl"":""https://48fc04bd-c84b-44ac-91b7-a4c5eefd5ac1.rms.na.aadrm.com/_wmcs/certification"",""pubk"":{""e"":""65537"",""kty"":""RSA"",""n"":""oxyxbUguymFHLhu80EH9m62c2XLFXbjm87KlEbSFQkieurr8RZgvtEMWdg6jMAkLJ7xPnWs3hEjOHw99irwZ1g5Z+6yYgktWlKibmPs5Lpamd6v53R40fKES/HNWdjbc68xN9tyfts4pOfyJxT5Xgo0rHVset+rDgShE9B3JQbY=""}},""issto"":{""em"":""t-joanto@microsoft.com"",""id"":{""typ"":""Federation"",""val"":""62012187-8d83-4d8a-b996-6af37b0f1d54""},""pubk"":{""e"":""65537"",""kty"":""RSA"",""n"":""ZeLjkvbnkVOUoeso21kZUcWhnDTh/ddwrp0+NKj8aqfyEHjfp2Jc38fW9LbWhqPBWjrad9cBdKkk7RHxFYZwIQXbDAPa18W6FvfAL67Bq9ETZ+0ugO2Gb66SYXH7VFg3bQHPF37l0sVaMKluxv3nY1ZDnlLDsMidHAwyzCdh1MU=""}}},""sig"":{""alg"":""SHA1"",""dig"":""CELn+qiDYKrSREHAdu5EQluTgKcjOGgDo7GtoQIJihCc9F3CUASUFnEWYfCRPoghWKZ2/hCWENAJWVnNDqhy1RHlWt+jl1S4I1CdHPSRj8Rp5IbiEAoXHAviHrns47OTLq5dWPdQ8BN4DtfSSTqNBoTI3QdFQW8d8C55GxjYjz4="",""penc"":""utf-8""}}}},""signed_data_hash"":""j/WbpShglfBevSmCi8ijdsqlpnd20wthAJgjBtFM2Zz8G1cf7svdyPHF3Aq6CAeVBqVpOHePCR/LVcEjnPy0qOetuCnDblfTRhQIGt1iUoODVg0nHMV7gfwQChyQnzChu53kv9VNYANpna3YbTwok2af09EHjhoyWxSh0BzLnRM=""}}";
            DataCollectionExp dcExp = new DataCollectionExp(json);
            Assert.IsFalse(dcExp.IsVerified());
        }

        [TestMethod]
        public void TestContainsKey()
        {
            String json = @"{""data"":{""Key"":""Value""},""signature"":{""algorithm"":"""",""certificate"":{""pld"":{""pub"":{""pld"":{""iss"":{""eurl"":""https://48fc04bd-c84b-44ac-91b7-a4c5eefd5ac1.rms.na.aadrm.com/_wmcs/certification"",""fname"":""CorpRights"",""id"":""{aaa331f3-e659-4ea9-876b-e8b25c7b4996}"",""iurl"":""https://48fc04bd-c84b-44ac-91b7-a4c5eefd5ac1.rms.na.aadrm.com/_wmcs/certification"",""pubk"":{""e"":""65537"",""kty"":""RSA"",""n"":""oxyxbUguymFHLhu80EH9m62c2XLFXbjm87KlEbSFQkieurr8RZgvtEMWdg6jMAkLJ7xPnWs3hEjOHw99irwZ1g5Z+6yYgktWlKibmPs5Lpamd6v53R40fKES/HNWdjbc68xN9tyfts4pOfyJxT5Xgo0rHVset+rDgShE9B3JQbY=""}},""issto"":{""em"":""t-joanto@microsoft.com"",""id"":{""typ"":""Federation"",""val"":""62012187-8d83-4d8a-b996-6af37b0f1d54""},""pubk"":{""e"":""65537"",""kty"":""RSA"",""n"":""ZeLjkvbnkVOUoeso21kZUcWhnDTh/ddwrp0+NKj8aqfyEHjfp2Jc38fW9LbWhqPBWjrad9cBdKkk7RHxFYZwIQXbDAPa18W6FvfAL67Bq9ETZ+0ugO2Gb66SYXH7VFg3bQHPF37l0sVaMKluxv3nY1ZDnlLDsMidHAwyzCdh1MU=""}}},""sig"":{""alg"":""SHA1"",""dig"":""CELn+qiDYKrSREHAdu5EQluTgKcjOGgDo7GtoQIJihCc9F3CUASUFnEWYfCRPoghWKZ2/hCWENAJWVnNDqhy1RHlWt+jl1S4I1CdHPSRj8Rp5IbiEAoXHAviHrns47OTLq5dWPdQ8BN4DtfSSTqNBoTI3QdFQW8d8C55GxjYjz4="",""penc"":""utf-8""}}}},""signed_data_hash"":""j/WbpShglfBevSmCi8ijdsqlpnd20wthAJgjBtFM2Zz8G1cf7svdyPHF3Aq6CAeVBqVpOHePCR/LVcEjnPy0qOetuCnDblfTRhQIGt1iUoODVg0nHMV7gfwQChyQnzChu53kv9VNYANpna3YbTwok2af09EHjhoyWxSh0BzLnRM=""}}";
            DataCollectionExp dcExp = new DataCollectionExp(json);
            Assert.IsTrue(dcExp.ContainsKey("Key"));
            Assert.IsFalse(dcExp.ContainsKey("Key2"));
        }

        [TestMethod]
        public void TestGetKey()
        {
            String json = @"{""data"":{""Key"":""Value""},""signature"":{""algorithm"":"""",""certificate"":{""pld"":{""pub"":{""pld"":{""iss"":{""eurl"":""https://48fc04bd-c84b-44ac-91b7-a4c5eefd5ac1.rms.na.aadrm.com/_wmcs/certification"",""fname"":""CorpRights"",""id"":""{aaa331f3-e659-4ea9-876b-e8b25c7b4996}"",""iurl"":""https://48fc04bd-c84b-44ac-91b7-a4c5eefd5ac1.rms.na.aadrm.com/_wmcs/certification"",""pubk"":{""e"":""65537"",""kty"":""RSA"",""n"":""oxyxbUguymFHLhu80EH9m62c2XLFXbjm87KlEbSFQkieurr8RZgvtEMWdg6jMAkLJ7xPnWs3hEjOHw99irwZ1g5Z+6yYgktWlKibmPs5Lpamd6v53R40fKES/HNWdjbc68xN9tyfts4pOfyJxT5Xgo0rHVset+rDgShE9B3JQbY=""}},""issto"":{""em"":""t-joanto@microsoft.com"",""id"":{""typ"":""Federation"",""val"":""62012187-8d83-4d8a-b996-6af37b0f1d54""},""pubk"":{""e"":""65537"",""kty"":""RSA"",""n"":""ZeLjkvbnkVOUoeso21kZUcWhnDTh/ddwrp0+NKj8aqfyEHjfp2Jc38fW9LbWhqPBWjrad9cBdKkk7RHxFYZwIQXbDAPa18W6FvfAL67Bq9ETZ+0ugO2Gb66SYXH7VFg3bQHPF37l0sVaMKluxv3nY1ZDnlLDsMidHAwyzCdh1MU=""}}},""sig"":{""alg"":""SHA1"",""dig"":""CELn+qiDYKrSREHAdu5EQluTgKcjOGgDo7GtoQIJihCc9F3CUASUFnEWYfCRPoghWKZ2/hCWENAJWVnNDqhy1RHlWt+jl1S4I1CdHPSRj8Rp5IbiEAoXHAviHrns47OTLq5dWPdQ8BN4DtfSSTqNBoTI3QdFQW8d8C55GxjYjz4="",""penc"":""utf-8""}}}},""signed_data_hash"":""j/WbpShglfBevSmCi8ijdsqlpnd20wthAJgjBtFM2Zz8G1cf7svdyPHF3Aq6CAeVBqVpOHePCR/LVcEjnPy0qOetuCnDblfTRhQIGt1iUoODVg0nHMV7gfwQChyQnzChu53kv9VNYANpna3YbTwok2af09EHjhoyWxSh0BzLnRM=""}}";
            DataCollectionExp dcExp = new DataCollectionExp(json);
            Assert.AreEqual(dcExp.GetDataValue("Key"), "Value");
        }

    }
}
