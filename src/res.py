import matplotlib.pyplot as plt

# 示例数据 (请替换为实际数据)
cache_sizes = [8, 64, 512, 4096, 8192]
insert_times = {
    1: [206642035, 192433429, 102816297, 67043842, 66075270],
    2: [341357078, 343609578, 188025879, 86763365, 81485683],
    4: [237670202, 228849574, 145070547, 104801273, 105133251],
    8: [64732452, 60366108, 35389174, 22902501, 22215264],
}
get_times = {
    1: [55393785, 56626919, 60383822, 61912904, 62621912],
    2: [66092669, 70916298, 74782289, 70763104, 70435036],
    4: [82932153, 88815339, 98111303, 90432477, 90025454],
    8: [19880536, 19165740, 20890489, 21267019, 20891867],
}
mixed_times = {
    1: [137660886, 133987107, 91515074, 69826488, 71490531],
    2: [249127892, 250388287, 149545641, 80901831, 92627963],
    4: [175652278, 171574906, 125470752, 106985344, 109208808],
    8: [43123291, 41921347, 28923594, 24178150, 23798911],
}

# 绘制插入操作的折线图
for threads, times in insert_times.items():
    plt.plot(cache_sizes, times, label=f'Threads: {threads}')
plt.xlabel('Cache Size')
plt.ylabel('Insert Time (ns)')
plt.title('Insert Operation Performance')
plt.legend()
plt.show()

# 绘制获取操作的折线图
for threads, times in get_times.items():
    plt.plot(cache_sizes, times, label=f'Threads: {threads}')
plt.xlabel('Cache Size')
plt.ylabel('Get Time (ns)')
plt.title('Get Operation Performance')
plt.legend()
plt.show()

# 绘制混合操作的折线图
for threads, times in mixed_times.items():
    plt.plot(cache_sizes, times, label=f'Threads: {threads}')
plt.xlabel('Cache Size')
plt.ylabel('Mixed Operation Time (ns)')
plt.title('Mixed Operation Performance')
plt.legend()
plt.show()
