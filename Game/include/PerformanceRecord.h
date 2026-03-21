
#pragma once
#include <string_view>

#include "nlohmann/json_fwd.hpp"

struct DataRecord
{
public:
	std::string Card;
	std::string OS;
	std::string RAM;
	std::string CPU;
	std::string API;
	std::string TenPointsGeneration;
	std::string TenPointsClipping;
	std::string OneHundredPointsGeneration;
	std::string OneHundredPointsClipping;
	std::string OneThousandPointsGeneration;
	std::string OneThousandPointsClipping;
};

class DataRecorder
{
public:
	static void SaveDataRecord(const DataRecord& Record, const std::string& Name);


	static bool DoesJsonContainRecord(const DataRecord& Record, const nlohmann::json& Root, size_t& Index);

	static void AppendRecordToData(const DataRecord& Record, nlohmann::ordered_json& Root, size_t& Index, const std::string_view& Name);

	static void MakeDataEntryArrayIfNot(nlohmann::ordered_json& Data, const std::string_view& Key);

	static void CopyFileContents(nlohmann::ordered_json& Destination, std::ifstream& Source);

	static bool CheckJsonAgainstData(const nlohmann::json Json, const DataRecord& Record);

	static void CreateNewDataEntry(const DataRecord& Record, nlohmann::ordered_json& root, const std::string_view& Name);

};
