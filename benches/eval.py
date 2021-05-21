import os
import re

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

for benchmark in ["short", "heavy"]:
  data = []
  for dirname in ["ts", "tts", "tts_pad"]:
    dir_path = benchmark + "/" + dirname
    for filename in os.listdir(dir_path):
      num_threads = re.findall("\d*", filename)[0]
      with open(dir_path + "/" + filename, 'r') as f:
        content = f.readlines()
      for line in content:
        data.append(
          {
            'type': dirname,
            'threads': int(num_threads),
            'rt': int(re.findall("\d*", line)[0]),
          })

  df = pd.DataFrame(data)
  sns_plot = sns.pointplot(data=df, x="threads", y="rt", hue="type", err_style="bars", palette="deep", dodge=True, join=False)
  sns_plot.set_xticks([x for x in range(2, 33, 4)])
  sns_plot.set(xlabel='Threads [#]', ylabel='Runtime [ms]')
  sns_plot.figure.savefig("{}.png".format(benchmark), dpi=1200)
  plt.clf()

  zoom_levels = [
    (2, 5),
    (5, 8),
    (8, 11),
    (11, 13),
    (13, 16),
    (16, 19),
    (19, 22),
    (22, 25),
    (25, 28),
    (28, 32)
  ]

  for level in zoom_levels:
    df2 = df.loc[(df['threads'] >= level[0]) & (df['threads'] <= level[1])]
    sns_plot = sns.pointplot(data=df2, x="threads", y="rt", hue="type", err_style="bars", palette="deep", dodge=True, join=False)
    sns_plot.set(xlabel='Threads [#]', ylabel='Runtime [ms]')
    sns_plot.figure.savefig("{}-{}-{}.png".format(benchmark, level[0], level[1]), dpi=1200)
    plt.clf()

  for threads in range(2, 33):
    df2 = df.loc[df['threads'] == threads]
    sns_plot = sns.violinplot(x="threads", y="rt", hue="type", inner="quartiles", data=df2, palette="deep")
    sns_plot.set(xlabel='Threads [#]', ylabel='Runtime [ms]')
    sns_plot.figure.savefig("{}-{}-violin.png".format(benchmark, threads), dpi=1200)
    plt.clf()
