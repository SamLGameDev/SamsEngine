#include "PerformanceRecord.h"
#include <exception>
#include <iostream>
#include "WorldObject.h"
#include "ObjectFactory.h"
#include <nlohmann/json.hpp>
#include <fstream>

#include "CorePaths.h"


void DataRecorder::SaveDataRecord(const DataRecord& Record, const std::string& Name)
{
	
    nlohmann::ordered_json root;

    const std::string directory = CorePaths::Contents.Path + Name;

	std::ifstream inFile(directory);

    CopyFileContents(root, inFile);

	if (!root.is_array())
    {
        root = nlohmann::json::array();
    }

    size_t index;

    if (DoesJsonContainRecord(Record, root, index))
    {
        AppendRecordToData(Record, root, index, Name);
        return;
    }

    CreateNewDataEntry(Record, root, Name);

}

bool DataRecorder::DoesJsonContainRecord(const DataRecord& Record, const nlohmann::json& Root, size_t& Index)
{
    for (size_t i = 0; i < Root.size(); i++)
    {
        if (Root[i].contains("Data"))
        {
            const auto& data = Root[i]["Data"];

            if (CheckJsonAgainstData(data, Record))
            {
                Index = i;
            	return true;
            }

        }
    }
    return false;
}

void DataRecorder::AppendRecordToData(const DataRecord& Record, nlohmann::ordered_json& Root, size_t& Index, const std::string_view& Name)
{
    nlohmann::ordered_json& data = Root[Index]["Data"];
    if (!Record.TenPointsGeneration.empty())
    {
        MakeDataEntryArrayIfNot(data, "10Gen");
        data["10Gen"].push_back(Record.TenPointsGeneration.data());
    }
    if (!Record.TenPointsClipping.empty())
    {
        MakeDataEntryArrayIfNot(data, "10Clip");
        data["10Clip"].push_back(Record.TenPointsClipping.data());
	}

    if (!Record.OneHundredPointsGeneration.empty()) 
    {
        MakeDataEntryArrayIfNot(data, "100Gen");
        data["100Gen"].push_back(Record.OneHundredPointsGeneration.data());
    }
    if (!Record.OneHundredPointsClipping.empty()) 
    {
        MakeDataEntryArrayIfNot(data, "100Clip");
        data["100Clip"].push_back(Record.OneHundredPointsClipping.data());
	}
    if (!Record.OneThousandPointsGeneration.empty()) 
    {
        MakeDataEntryArrayIfNot(data, "1000Gen");

        data["1000Gen"].push_back(Record.OneThousandPointsGeneration.data());
    }
    if (!Record.OneThousandPointsClipping.empty())
    {
        MakeDataEntryArrayIfNot(data, "1000Clip");
        data["1000Clip"].push_back(Record.OneThousandPointsClipping.data());
    }
    std::ofstream file(CorePaths::Contents.Path + Name.data());
    file << Root.dump(4);
    file.close();
}

void DataRecorder::MakeDataEntryArrayIfNot(nlohmann::ordered_json& Data, const std::string_view& Key)
{
    if (!Data[Key].is_array())
    {
        nlohmann::ordered_json info = Data[Key];
        Data[Key] = nlohmann::json::array();
        Data[Key].push_back(info);
    }
}

void DataRecorder::CopyFileContents(nlohmann::ordered_json& Destination, std::ifstream& Source)
{
    if (Source.is_open())
    {
        try
        {
            Source >> Destination;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error reading JSON file: " << e.what() << std::endl;
        }
        Source.close();
    }
}

bool DataRecorder::CheckJsonAgainstData(const nlohmann::json Json, const DataRecord& Record)
{
	if (Json["Card"] != Record.Card.data()) return false;
	if (Json["OS"] != Record.OS.data()) return false;
	if (Json["RAM"] != Record.RAM.data()) return false;
	if (Json["CPU"] != Record.CPU.data()) return false;
	if (Json["API"] != Record.API.data()) return false;
    return true;
}

void DataRecorder::CreateNewDataEntry(const DataRecord& Record, nlohmann::ordered_json& root, const std::string_view& Name)
{
    nlohmann::ordered_json jsonData;

    jsonData["Card"] = Record.Card;
    jsonData["OS"] = Record.OS;
    jsonData["RAM"] = Record.RAM;
    jsonData["CPU"] = Record.CPU;
    jsonData["API"] = Record.API;

    if (!Record.TenPointsGeneration.empty()) jsonData["10Gen"] = Record.TenPointsGeneration;
	if (!Record.TenPointsClipping.empty()) jsonData["10Clip"] = Record.TenPointsClipping;
    if (!Record.OneHundredPointsGeneration.empty()) jsonData["100Gen"] = Record.OneHundredPointsGeneration;
	if (!Record.OneHundredPointsClipping.empty()) jsonData["100Clip"] = Record.OneHundredPointsClipping;
    if (!Record.OneThousandPointsGeneration.empty()) jsonData["1000Gen"] = Record.OneThousandPointsGeneration;
	if (!Record.OneThousandPointsClipping.empty()) jsonData["1000Clip"] = Record.OneThousandPointsClipping;

    nlohmann::ordered_json newRoot;
    newRoot["Data"] = jsonData;

    root.push_back(newRoot);

    std::ofstream file(CorePaths::Contents.Path + Name.data());
    file << root.dump(4);
    file.close();
}
