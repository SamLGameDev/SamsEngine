
#pragma once
#include <string_view>

#include "Array.h"
#include "nlohmann/json_fwd.hpp"

struct PointEntry
{
public:
	std::string NumPoints;
	std::string Generation;
	std::string Clipping;
};

struct DataRecord
{
public:
	std::string Card;
	std::string OS;
	std::string RAM;
	std::string CPU;
	std::string API;

	Array<PointEntry> PointInfo;

};

class DataRecorder
{
public:
	static void SaveDataRecord(const DataRecord& Record, const std::string& Name);


	static bool DoesJsonContainRecord(const DataRecord& Record, const nlohmann::json& Root, size_t& Index);

	static void AppendRecordToData(const DataRecord& Record, nlohmann::ordered_json& Root, size_t& Index, const std::string_view& Name);
	static void AppendRecordToData(const DataRecord& Record, nlohmann::ordered_json& Root, const std::string_view& Name);

	static void MakeDataEntryArrayIfNot(nlohmann::ordered_json& Data, const std::string_view& Key);

	static void CopyFileContents(nlohmann::ordered_json& Destination, std::ifstream& Source);

	static bool CheckJsonAgainstData(const nlohmann::json Json, const DataRecord& Record);

	static void CreateNewDataEntry(const DataRecord& Record, nlohmann::ordered_json& root, const std::string_view& Name);

};
