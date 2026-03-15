import pandas as pd
from scipy.stats import mannwhitneyu
import matplotlib.pyplot as plt
import textwrap

def wrap(s, width=20):
    return "\n".join(textwrap.wrap(s, width))

#Loads the data, and normalises it into a format that cen be used for testing
def LoadData(path):
    data = pd.read_json(path)
    data = pd.json_normalize(data["Data"])
    return data

#Runs the Wilcoxon-Mann Whitney test, returning the results
def RunTest(vulaknTimes, openGLTimes):
    return mannwhitneyu(vulaknTimes, openGLTimes)

#Creates a box plot from the vulkan times and opengl times, Only for one set
def CreateBoxPlot(vulkan_data, opengl_data, VulkanLaptop100, OpenGLLaptop100, VulkanGA10, OpenGLGA10, VulkanGA100, OpenGLGA100):
    plot_df = pd.DataFrame({
        "Time taken": list(vulkan_data) + list(opengl_data) + list(VulkanLaptop100) + list(OpenGLLaptop100) +
                      list(VulkanGA10) + list(OpenGLGA10)+ list(VulkanGA100) + list(OpenGLGA100),
        "API": [wrap("Vulkan, CellNumber: 10: GPU: NVIDIA GeForce RTX 4080 Laptop GPU")] * len(vulkan_data)
               + [wrap("OpenGL, CellNumber: 10: GPU: NVIDIA GeForce RTX 4080 Laptop GPU")] * len(opengl_data)
               + [wrap("Vulkan, CellNumber: 100: GPU: NVIDIA GeForce RTX 4080 Laptop GPU")] * len(VulkanGA100)
               + [wrap("OpenGL, CellNumber: 100: GPU: NVIDIA GeForce RTX 4080 Laptop GPU")] * len(OpenGLLaptop100)
               + [wrap("Vulkan, CellNumber: 10: GPU: NVIDIA GeForce RTX 5070")] * len(VulkanGA10)
               + [wrap("OpenGL, CellNumber: 10: GPU: NVIDIA GeForce RTX 5070")] * len(OpenGLGA10)
               + [wrap("Vulkan, CellNumber: 100: GPU: NVIDIA GeForce RTX 5070")] * len(VulkanGA100)
               + [wrap("OpenGL, CellNumber: 100: GPU: NVIDIA GeForce RTX 5070")] * len(OpenGLGA100)
    })

    plot_df.boxplot(column="Time taken", by="API")

    plt.title("Time taken by API")
    plt.suptitle("Cell Clipping")
    plt.show()
    return

def main():

    data = LoadData("CellGenerationResults.json")

    Set = input("Clip or Gen")
    Key = "10" + Set
    Card = "NVIDIA GeForce RTX 4080 Laptop GPU"
    vulkan_api = data[(data["API"] == "Vulkan") & (data["Card"] == Card)][Key].iloc()[0]
    vulkan_opengl = data[(data["API"] == "OpenGL") & (data["Card"] == Card)][Key].iloc()[0]

    Card = "NVIDIA GeForce RTX 5070"

    vulkan_api_ga = data[(data["API"] == "Vulkan") & (data["Card"] == Card)][Key].iloc()[0]
    api_opengl_ga = data[(data["API"] == "OpenGL") & (data["Card"] == Card)][Key].iloc()[0]

    Key = "100" + Set

    vulkan_100_ga = data[(data["API"] == "Vulkan") & (data["Card"] == Card)][Key].iloc()[0]
    opengl_100_ga = data[(data["API"] == "OpenGL") & (data["Card"] == Card)][Key].iloc()[0]
    Card = "NVIDIA GeForce RTX 4080 Laptop GPU"
    vulkan_100 = data[(data["API"] == "Vulkan") & (data["Card"] == Card)][Key].iloc()[0]
    opengl_100 = data[(data["API"] == "OpenGL") & (data["Card"] == Card)][Key].iloc()[0]

    vulkan_api = pd.to_numeric(vulkan_api)
    vulkan_opengl = pd.to_numeric(vulkan_opengl)
    vulkan_api_ga = pd.to_numeric(vulkan_api_ga)
    api_opengl_ga = pd.to_numeric(api_opengl_ga)
    vulkan_100_ga = pd.to_numeric(vulkan_100_ga)
    opengl_100_ga = pd.to_numeric(opengl_100_ga)
    vulkan_100 = pd.to_numeric(vulkan_100)
    opengl_100 = pd.to_numeric(opengl_100)

    file =  open("WhitneyTestResults.txt", "a")
    test = RunTest(vulkan_api, vulkan_opengl)
    Key = "10"
    file.write("Computer: " + Card + ", Operation: "  + Set + ", Set: " + str(Key) + ", statistic: " + str(test.statistic) + ", p_value: "+ str(test.pvalue) + ", Significance: " + ("True" if test.pvalue < 0.15/8 else "False") + "\n")
    test = RunTest(vulkan_100, opengl_100)
    Key = "100"
    file.write("Computer: " + Card + ", Operation: "  + Set + ", Set: " + str(Key) + ", statistic: " + str(test.statistic) + ", p_value: "+ str(test.pvalue)+ ", Significance: " + ("True" if test.pvalue < 0.15/8 else "False") + "\n")
    Card = "NVIDIA GeForce RTX 5070"
    test = RunTest(vulkan_api_ga, api_opengl_ga)
    Key = "10"
    file.write("Computer: " + Card + ", Operation: "  + Set + ", Set: " + str(Key) + ", statistic: " + str(test.statistic) + ", p_value: "+ str(test.pvalue) + ", Significance: " + ("True" if test.pvalue < 0.15/8 else "False") + "\n")
    test = RunTest(vulkan_100_ga, opengl_100_ga)
    Key = "100"
    file.write("Computer: " + Card + ", Operation: "  + Set + ", Set: " + str(Key) + ", statistic: " + str(test.statistic) + ", p_value: "+ str(test.pvalue)+ ", Significance: " + ("True" if test.pvalue < 0.15/8 else "False") + "\n")

    file.flush()

    CreateBoxPlot(vulkan_api, vulkan_opengl, vulkan_100, opengl_100, vulkan_api_ga, api_opengl_ga, vulkan_100_ga, opengl_100_ga)

if __name__ == '__main__':
    main()
