import time
from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt

from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import precision_score, recall_score, f1_score


DATA_PATH = Path("data/telemetry_data.csv")

FEATURE_COLS = [
    "Temperature",
    "Cpu_usage",
    "Latency_ms",
    "Error_count",
]

TARGET_COL = "Is_anomaly"

MIN_ROWS = 50
EVAL_INTERVAL = 50
POLL_INTERVAL = 1.0


def poll_csv(path, poll_interval=1.0):
    """
    Continuously watches a CSV file and yields only newly added rows.

    This version starts polling after the rows that already exist,
    so it does not duplicate initial data.
    """
    path = Path(path)

    if path.exists():
        try:
            initial_df = pd.read_csv(path, on_bad_lines="skip")
            last_row_count = len(initial_df)
        except pd.errors.EmptyDataError:
            last_row_count = 0
    else:
        last_row_count = 0

    while True:
        if not path.exists():
            print("Waiting for CSV file...")
            time.sleep(poll_interval)
            continue

        try:
            df = pd.read_csv(path, on_bad_lines="skip")
        except pd.errors.EmptyDataError:
            time.sleep(poll_interval)
            continue

        if len(df) > last_row_count:
            new_rows = df.iloc[last_row_count:].copy()
            last_row_count = len(df)
            yield new_rows

        time.sleep(poll_interval)



def calculate_average_confidence(y_true, probabilities, classes):
    y_true = y_true.to_numpy()

    normal_col = list(classes).index(0)
    anomaly_col = list(classes).index(1)

    normal_probs = probabilities[:, normal_col]
    anomaly_probs = probabilities[:, anomaly_col]

    anomaly_mask = y_true == 1
    normal_mask = y_true == 0

    avg_anomaly_conf = anomaly_probs[anomaly_mask].mean() if anomaly_mask.any() else 0.0
    avg_normal_conf = normal_probs[normal_mask].mean() if normal_mask.any() else 0.0

    return avg_anomaly_conf, avg_normal_conf
    
    

    return


def clean_dataframe(df):
    """
    Keeps only the needed columns and drops rows with missing/bad values.
    """
    required_cols = FEATURE_COLS + [TARGET_COL]

    missing_cols = [col for col in required_cols if col not in df.columns]
    if missing_cols:
        raise ValueError(f"Missing required columns: {missing_cols}")

    cleaned = df[required_cols].copy()

    for col in FEATURE_COLS:
        cleaned[col] = pd.to_numeric(cleaned[col], errors="coerce")

    cleaned[TARGET_COL] = pd.to_numeric(cleaned[TARGET_COL], errors="coerce")

    cleaned = cleaned.dropna()

    cleaned[TARGET_COL] = cleaned[TARGET_COL].astype(int)

    return cleaned


def perform_anomaly_detection(df):
    """
    Trains logistic regression on the current accumulated dataset
    and evaluates on a random held-out test split.
    """
    df = clean_dataframe(df)

    X = df[FEATURE_COLS]
    y = df[TARGET_COL]

    X_train, X_test, y_train, y_test = train_test_split(
        X,
        y,
        test_size=0.2,
        random_state=42,
        stratify=y if y.nunique() == 2 else None,
    )

    model = LogisticRegression(max_iter=1000)
    model.fit(X_train, y_train)

    #predictions = model.predict(X_test)

    return y_test, model.predict_proba(X_test), model.classes_


def eval_summary(df):
    y_true, predictions, classes = perform_anomaly_detection(df)

    one_conf_avg, zero_conf_avg = calculate_average_confidence(y_true, predictions, classes) #might need [:,1] to get the anomaly confidence

    print("-" * 40)
    print(f"Rows seen: {len(df)}")
    print(f"Average confidence for anomalies: {one_conf_avg:.4f}")
    print(f"Average confidence for normal instances: {zero_conf_avg:.4f}")

    return one_conf_avg, zero_conf_avg

def update_plot(sample_counts,one_conf_history, zero_conf_history):
    plt.clf()

    plt.plot(sample_counts, one_conf_history, label="Average Confidence (Anomalies)")
    plt.plot(sample_counts, zero_conf_history, label="Average Confidence (Normal)")

    plt.xlabel("Rows seen")
    plt.ylabel("Confidence")
    plt.title("Live Logistic Regression Performance Over Time")
    plt.ylim(0, 1.05)
    plt.legend()
    plt.grid(True)

    plt.pause(0.1)


def main():
    if DATA_PATH.exists():
        try:
            all_data = pd.read_csv(DATA_PATH, on_bad_lines="skip")
        except pd.errors.EmptyDataError:
            all_data = pd.DataFrame()
    else:
        all_data = pd.DataFrame()

    sample_counts = []
    one_conf_history = []
    zero_conf_history = []

    last_eval_count = 0

    plt.ion()
    plt.figure(figsize=(10, 6))

    print(f"Starting live monitor for: {DATA_PATH}")
    print("Waiting for new telemetry rows...")

    for new_data in poll_csv(DATA_PATH, poll_interval=POLL_INTERVAL):
        print(f"New rows detected: {len(new_data)}")

        all_data = pd.concat([all_data, new_data], ignore_index=True)

        try:
            cleaned_data = clean_dataframe(all_data)
        except ValueError as e:
            print(f"Data error: {e}")
            continue

        if len(cleaned_data) < MIN_ROWS:
            print(f"Not enough data yet: {len(cleaned_data)}/{MIN_ROWS}")
            continue

        if cleaned_data[TARGET_COL].nunique() < 2:
            print("Need both normal and anomaly examples before training.")
            continue

        if len(cleaned_data) - last_eval_count < EVAL_INTERVAL:
            print(
                f"Waiting for more rows before next eval: "
                f"{len(cleaned_data) - last_eval_count}/{EVAL_INTERVAL}"
            )
            continue

        last_eval_count = len(cleaned_data)

        one_conf_avg, zero_conf_avg = eval_summary(cleaned_data)

        sample_counts.append(len(cleaned_data))
        one_conf_history.append(one_conf_avg)
        zero_conf_history.append(zero_conf_avg)

        update_plot(
            sample_counts,
            one_conf_history,
            zero_conf_history,
        )


if __name__ == "__main__":
    main()