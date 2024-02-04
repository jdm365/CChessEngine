import pandas as pd



if __name__ == '__main__':
    ## df = pd.read_csv('evaluations.csv')
    df = pd.read_csv('evaluations.csv', nrows=100)

    print(df[['binary']], df.columns)
