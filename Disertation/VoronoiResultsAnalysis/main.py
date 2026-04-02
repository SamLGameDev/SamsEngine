import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import json
import statsmodels.formula.api as smf

def main():

    with open("TestData.json") as f:
        root = json.load(f)

    df = FormatData(root)

    #create a data set containing the mean of each point set
    df_avg = df.groupby(["NumPoints", "Type", "API", "Card"]).mean().reset_index()

    PerformRegressionTests(df_avg)

    PlotGenerationTimes(df_avg)

    PlotClippingTImes(df_avg)


def PerformRegressionTests(df_avg):

    #Get only the generation mean values
    df_gen = df_avg[df_avg["Type"] == "Gen"]

    #Create a model with the dependent variable of time, independed variables of NUmPoints, API and GPU.
    #Have three interaction terms between Numpoints and API, Numpoints and GPU, and API and GPU

    model = smf.ols(
        "Time ~ NumPoints + API + Card + NumPoints:API + NumPoints:Card + API:Card",
        data=df_gen).fit()

    print(model.summary())

    #Get only the Clipping mean values

    df_clip = df_avg[df_avg["Type"] == "Clip"]

    #Create a model with the dependent variable of time, independed variables of NUmPoints, API and GPU.
    #Have three interaction terms between Numpoints and API, Numpoints and GPU, and API and GPU

    model = smf.ols(
        "Time ~ NumPoints + API + Card + NumPoints:API + NumPoints:Card + API:Card",
        data=df_clip).fit()

    print(model.summary())


def PlotClippingTImes(df_avg):
    plt.figure()
    sns.lineplot(
        data=df_avg[df_avg["Type"] == "Clip"],
        x="NumPoints",
        y="Time",
        hue="API",
        style="Card",
        markers=True
    )
    plt.title("Clipping Time Comparison")
    plt.xlabel("Number of Points")
    plt.ylabel("Time (seconds)")
    plt.show()


def PlotGenerationTimes(df_avg):
    plt.figure()
    sns.lineplot(
        data=df_avg[df_avg["Type"] == "Gen"],
        x="NumPoints",
        y="Time",
        hue="API",
        style="Card",
        markers=True
    )
    plt.title("Generation Time Comparison")
    plt.xlabel("Number of Points")
    plt.ylabel("Time (seconds)")
    plt.show()


def FormatData(root):
    rows = []
    for entry in root:

        #Each entry starts with Data, so it needs to be changed to one level down
        entry = entry["Data"]


        card = entry["Card"]
        api = entry["API"]
        for key, value in entry.items():

            #If its not a recorded time, continue

            if key in ["Card", "OS", "RAM", "CPU", "API"]:
                continue

            num_points = int(key)

            for run in value:

                #Get all generation and clip values for that point number

                gen_vals = [float(x) for x in run["Gen"]]
                clip_vals = [float(x) for x in run["Clip"]]

                for g in gen_vals:
                    rows.append({
                        "NumPoints": num_points,
                        "Time": g,
                        "Type": "Gen",
                        "API": api,
                        "Card": card
                    })

                for c in clip_vals:
                    rows.append({
                        "NumPoints": num_points,
                        "Time": c,
                        "Type": "Clip",
                        "API": api,
                        "Card": card
                    })
    df = pd.DataFrame(rows)
    return df


if __name__ == '__main__':
    main()
