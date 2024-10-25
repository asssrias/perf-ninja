import cpuinfo

info = cpuinfo.get_cpu_info()
# cache_line_size = info['l1_data_cache_line_size']

for key, value in info.items():
        print("{0}: {1}".format(key, value))