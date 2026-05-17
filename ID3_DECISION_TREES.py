import pandas as pd
import numpy as np
from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, classification_report
from sklearn.metrics import confusion_matrix


# ─────────────────────────────────────────────
# Core entropy / information-gain helpers
# ─────────────────────────────────────────────

def entropy(target_col):
    """Shannon entropy of a label column."""
    elements, counts = np.unique(target_col, return_counts=True)
    probs = counts / counts.sum()
    return -np.sum(probs * np.log2(probs + 1e-9))   # 1e-9 avoids log(0)


def information_gain(data, feature, target="Class"):
    """Information gain of splitting `data` on `feature`."""
    total_entropy = entropy(data[target])
    values, counts = np.unique(data[feature], return_counts=True)
    weighted_entropy = np.sum([
        (counts[i] / counts.sum()) *
        entropy(data[data[feature] == values[i]][target])
        for i in range(len(values))
    ])
    return total_entropy - weighted_entropy


# ─────────────────────────────────────────────
# ID3 tree builder
# ─────────────────────────────────────────────

def id3(data, original_data, features, target="Class", parent_node_class=None):
    """
    Recursively build an ID3 decision tree.

    Returns a nested dict:  { feature: { value: subtree | leaf, ... } }
    A leaf is just the predicted class label (string/int).
    """
    # 1. All samples belong to one class → leaf
    if len(np.unique(data[target])) == 1:
        return np.unique(data[target])[0]

    # 2. No samples left → majority class of the parent
    if len(data) == 0:
        return np.unique(original_data[target])[
            np.argmax(np.unique(original_data[target], return_counts=True)[1])
        ]

    # 3. No features left → majority class of current node
    if len(features) == 0:
        return np.unique(data[target])[
            np.argmax(np.unique(data[target], return_counts=True)[1])
        ]

    # Default prediction for this node
    parent_node_class = np.unique(data[target])[
        np.argmax(np.unique(data[target], return_counts=True)[1])
    ]

    # 4. Choose the best feature (highest information gain)
    gains = {f: information_gain(data, f, target) for f in features}
    best_feature = max(gains, key=gains.get)

    # 5. Build the sub-tree
    tree = {best_feature: {}}
    remaining_features = [f for f in features if f != best_feature]

    for value in np.unique(data[best_feature]):
        subset = data[data[best_feature] == value].reset_index(drop=True)
        subtree = id3(subset, original_data, remaining_features, target, parent_node_class)
        tree[best_feature][value] = subtree

    return tree


# ─────────────────────────────────────────────
# Prediction
# ─────────────────────────────────────────────

def predict_sample(tree, sample, default=None):
    """Walk the tree for a single sample (pd.Series or dict)."""
    if not isinstance(tree, dict):
        return tree                          # leaf node

    feature = next(iter(tree))
    value = sample.get(feature) if isinstance(sample, dict) else sample[feature]

    subtree = tree[feature].get(value)
    if subtree is None:
        return default                       # unseen value → fallback
    return predict_sample(subtree, sample, default)


def predict(tree, data, default=None):
    """Predict labels for an entire DataFrame."""
    return data.apply(lambda row: predict_sample(tree, row, default), axis=1)


# ─────────────────────────────────────────────
# Continuous feature discretisation (needed for ID3)
# ─────────────────────────────────────────────

def discretise(df, features, n_bins=3):
    """
    Replace continuous columns with bin labels so ID3 can split on them.
    Uses equal-frequency (quantile) bins.
    """
    df = df.copy()
    bin_edges = {}
    for col in features:
        df[col], edges = pd.qcut(df[col], q=n_bins, labels=False,
                                 retbins=True, duplicates="drop")
        bin_edges[col] = edges
    return df, bin_edges


def apply_bins(df, bin_edges):
    """Apply pre-computed bin edges (from training) to a new DataFrame."""
    df = df.copy()
    for col, edges in bin_edges.items():
        df[col] = pd.cut(df[col], bins=edges, labels=False, include_lowest=True)
    return df


# ─────────────────────────────────────────────
# Pretty-print the tree
# ─────────────────────────────────────────────

def print_tree(tree, indent=0):
    if not isinstance(tree, dict):
        print("  " * indent + f"→ Predict: {tree}")
        return
    feature = next(iter(tree))
    print("  " * indent + f"[{feature}]")
    for value, subtree in tree[feature].items():
        print("  " * (indent + 1) + f"= {value}:")
        print_tree(subtree, indent + 2)


# ─────────────────────────────────────────────
# Main: load Iris, train, evaluate
# ─────────────────────────────────────────────

def main():
    discretise_flag = False
    # --- Load dataframe
    
    df = pd.read_csv("Bata_dase.csv")
    
    df = df.frame.rename(columns={"jogada": "Class"})

    
    feature_cols = list(df.feature_names)

    # --- Train / test split (80 / 20, stratified) ---
    train_df, test_df = train_test_split(df, test_size=0.2,
                                         random_state=42, stratify=df["Class"])
    train_df = train_df.reset_index(drop=True)
    test_df  = test_df.reset_index(drop=True)

    # --- Discretise continuous features (fit on train only) ---
    if discretise_flag:
        train_disc, bin_edges = discretise(train_df, feature_cols, n_bins=3)
        test_disc  = apply_bins(test_df, bin_edges)
    # Drop rows where discretisation produced NaN (rare edge-cases)
    test_disc  = test_disc.dropna(subset=feature_cols)
    test_df    = test_df.loc[test_disc.index].reset_index(drop=True)
    test_disc  = test_disc.reset_index(drop=True)

    # --- Build the ID3 tree ---
    print("=" * 60)
    print("Building ID3 tree …")
    print("=" * 60)
    tree = id3(train_disc, train_disc, feature_cols, target="Class")
    print_tree(tree)
    print()

    # --- Evaluate on the test set ---
    default_class = train_df["Class"].value_counts().idxmax()
    predictions = predict(tree, test_disc, default=default_class)

    acc = accuracy_score(test_df["Class"], predictions)
    print("=" * 60)
    print(f"Test accuracy: {acc * 100:.2f}%  ({int(acc * len(test_df))}/{len(test_df)} correct)")
    print("=" * 60)
    print(classification_report(test_df["Class"], predictions, zero_division=0))

    return tree, dataset_entropies


if __name__ == "__main__":
    tree, entropies = main()