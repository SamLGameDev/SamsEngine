
#pragma once
#include <string_view>

#include "nlohmann/json_fwd.hpp"

constexpr std::string_view SaveLocation = "/TestData/TestData.json";

struct DataRecord
{
public:
	std::string_view Card;
	std::string_view OS;
	std::string_view RAM;
	std::string_view CPU;
	std::string_view API;
	std::string_view TenPoints;
	std::string_view OneHundredPoints;
	std::string_view OneThousandPoints;
};

class DataRecorder
{
public:
	static void SaveDataRecord(const DataRecord& Record);


	static bool DoesJsonContainRecord(const DataRecord& Record, const nlohmann::json& Root, size_t& Index);

	static void AppendRecordToData(const DataRecord& Record, nlohmann::ordered_json& Root, size_t& Index);

	static void MakeDataEntryArrayIfNot(nlohmann::ordered_json& Data, const std::string_view& Key);

	static void CopyFileContents(nlohmann::ordered_json& Destination, std::ifstream& Source);

	static bool CheckJsonAgainstData(const nlohmann::json Json, const DataRecord& Record);

	static void CreateNewDataEntry(const DataRecord& Record, nlohmann::ordered_json& root);

};
