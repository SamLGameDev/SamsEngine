import pandas as pd
from scipy.stats import mannwhitneyu
import matplotlib.pyplot as plt

#Loads the data, and normalises it into a format that cen be used for testing
def LoadData(path):
    data = pd.read_json(path)
    data = pd.json_normalize(data["Data"])
    return data

#Runs the Wilcoxon-Mann Whitney test, returning the results
def RunTest(vulaknTimes, openGLTimes):
    return mannwhitneyu(vulaknTimes, openGLTimes)

#Creates a box plot from the vulkan times and opengl times, Only for one set
def CreateBoxPlot(vulkan_data, opengl_data):

    plot_df = pd.DataFrame({
        "Time taken": vulkan_data + opengl_data,
        "API": ["Vulkan"] * len(vulkan_data) + ["OpenGL"] * len(opengl_data)
    })

    plot_df.boxplot(column="Time taken", by="API")

    plt.title("Time taken by API")
    plt.suptitle("")
    plt.show()
    return

def main():

    data = LoadData("TestData.json")

    Key = input("Set to test: 10, 100, 1000")

    vulkan_api = data[data["API"] == "Vulkan"][Key].iloc()[0]
    vulkan_opengl = data[data["API"] == "OpenGL"][Key].iloc()[0]

    test = RunTest(vulkan_api, vulkan_opengl)

    file =  open("WhitneyTestResults.txt", "a")
    file.write("Set: " + str(Key) + " ,statistic: " + str(test.statistic) + " , p_value: "+ str(test.pvalue) + "\n")
    file.flush()

    CreateBoxPlot(vulkan_api, vulkan_opengl)

if __name__ == '__main__':
    main()
