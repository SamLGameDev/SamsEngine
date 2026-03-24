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

    for (const auto& set : Record.PointInfo)
    {
        MakeDataEntryArrayIfNot(data, set.NumPoints);

        nlohmann::ordered_json jsonData;

        jsonData["Gen"].push_back(set.Generation);
        jsonData["Clip"].push_back(set.Clipping);
        data[set.NumPoints].push_back(jsonData);
    }

    std::ofstream file(CorePaths::Contents.Path + Name.data());
    file << Root.dump(4);
    file.close();
}

void DataRecorder::AppendRecordToData(const DataRecord& Record, nlohmann::ordered_json& Root, const std::string_view& Name)
{
    for (const auto& set : Record.PointInfo)
    {
        MakeDataEntryArrayIfNot(Root, set.NumPoints);

        nlohmann::ordered_json jsonData;

        jsonData["Gen"].push_back(set.Generation);
        jsonData["Clip"].push_back(set.Clipping);
        Root[set.NumPoints].push_back(jsonData);
    }
}

void DataRecorder::MakeDataEntryArrayIfNot(nlohmann::ordered_json& Data, const std::string_view& Key)
{
    if (!Data.contains(Key))
    {
        Data[Key] = nlohmann::ordered_json::array();
    }
    else if (!Data[Key].is_array())
    {
        auto old = Data[Key];
        Data[Key] = nlohmann::ordered_json::array();
        Data[Key].push_back(old);
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

    AppendRecordToData(Record, jsonData, Name);

    nlohmann::ordered_json newRoot;
    newRoot["Data"] = jsonData;

    root.push_back(newRoot);

    std::ofstream file(CorePaths::Contents.Path + Name.data());
    file << root.dump(4);
    file.close();
}
