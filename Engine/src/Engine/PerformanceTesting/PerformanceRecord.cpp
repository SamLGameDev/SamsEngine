#include "PerformanceRecord.h"
#include <exception>
#include <iostream>
#include "WorldObject.h"
#include "ObjectFactory.h"
#include <nlohmann/json.hpp>
#include <fstream>

#include "CorePaths.h"


void DataRecorder::SaveDataRecord(const DataRecord& Record)
{
	
    nlohmann::ordered_json root;

	std::ifstream inFile(CorePaths::Contents.Path + SaveLocation.data());

    CopyFileContents(root, inFile);

	if (!root.is_array())
    {
        root = nlohmann::json::array();
    }

    size_t index;

    if (DoesJsonContainRecord(Record, root, index))
    {
        AppendRecordToData(Record, root, index);
        return;
    }

    CreateNewDataEntry(Record, root);

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

void DataRecorder::AppendRecordToData(const DataRecord& Record, nlohmann::ordered_json& Root, size_t& Index)
{
    nlohmann::ordered_json& data = Root[Index]["Data"];

    MakeDataEntryArrayIfNot(data, "10");

    MakeDataEntryArrayIfNot(data, "100");

	MakeDataEntryArrayIfNot(data, "1000");

    data["10"].push_back(Record.TenPoints.data());
    data["100"].push_back(Record.OneHundredPoints.data());
    data["1000"].push_back(Record.OneThousandPoints.data());

    std::ofstream file(CorePaths::Contents.Path + SaveLocation.data());
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

void DataRecorder::CreateNewDataEntry(const DataRecord& Record, nlohmann::ordered_json& root)
{
    nlohmann::ordered_json jsonData;

    jsonData["Card"] = Record.Card;
    jsonData["OS"] = Record.OS;
    jsonData["RAM"] = Record.RAM;
    jsonData["CPU"] = Record.CPU;
    jsonData["API"] = Record.API;
    jsonData["10"] = Record.TenPoints;
    jsonData["100"] = Record.OneHundredPoints;
    jsonData["1000"] = Record.OneThousandPoints;

    nlohmann::ordered_json newRoot;
    newRoot["Data"] = jsonData;

    root.push_back(newRoot);

    std::ofstream file(CorePaths::Contents.Path + SaveLocation.data());
    file << root.dump(4);
    file.close();
}
