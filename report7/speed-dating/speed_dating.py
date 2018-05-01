# coding: utf-8
import os.path
import gzip
import pickle
import os
import numpy as np
import pandas as pd
import sklearn.model_selection

dataset_dir = os.path.dirname(os.path.abspath(__file__))
save_file = dataset_dir + "/speed_dating.pkl"


def _load_X(X_df):
    return np.array(X_df)


def _load_y(y_df):
    return np.array(pd.to_numeric(y_df))


def _load_dataset(feature_selection=False):
    dataset_filepath = "/Users/akariasai/Projects/EEIC/AI/report7/speed_dating/full_matching_data.csv"
    full_matching_df = pd.read_csv(dataset_filepath)

    dataset = {}
    if feature_selection == True:
        cols = ['male_fun3_1', 'female_study_medical science, pharmaceuticals, and bio tech',
                'female_clubbing', 'male_3_1sum', 'female_art', 'female_fun1_1', 'male_clubbing',\
                'female_career_politics', 'female_shar2_1', 'female_study_social work', 'female_shar1_1', \
                'male_imprace', 'male_race_asian/pacific islander/asian-american', \
                'male_go_out', 'male_date']

    else:
        cols = list(set(full_matching_df.columns) - set(['match']))

    train_x, test_x, train_y, test_y = \
        sklearn.model_selection.train_test_split(
            full_matching_df[cols], full_matching_df.match, test_size=0.3)

    dataset['train_X'] = _load_X(train_x)
    dataset['train_y'] = _load_y(train_y)
    dataset['test_X'] = _load_X(test_x)
    dataset['test_y'] = _load_y(test_y)

    return dataset


def _change_one_hot_label(X):
    T = np.zeros((X.size, 2))
    for idx, row in enumerate(T):
        row[int(X[idx])] = 1

    return T


def load_speed_dating(normalize=True, flatten=True, one_hot_label=False, feature_selection=False):
    dataset = _load_dataset(feature_selection)

    if one_hot_label:
        dataset['train_y'] = _change_one_hot_label(dataset['train_y'])
        dataset['test_y'] = _change_one_hot_label(dataset['test_y'])

    return (dataset['train_X'], dataset['train_y']), (dataset['test_X'], dataset['test_y'])


if __name__ == '__main__':
    load_speed_dating()
