import re
import os

config = []
                # cand_max / do_dyn_cand. /  thresh / sel_size / do_dyn_sel /  thresh
config.append([40,          "False",          12.8,     20,        "False",     12.8])
config.append([80,          "False",          12.8,     20,        "False",     12.8])
config.append([160,         "False",          12.8,     20,        "False",     12.8])

messages="All one score experiment - kvmemn2n"

def sub(key, value):
    modified = re.sub(r'%s = .*' % (key), r'%s = %s' % (key, value), txt)
    return modified

def dyn_or_static(do_dyn_cand, do_dyn_sel):
    if do_dyn_cand == "False" and do_dyn_sel == "False":
        return "static"
    else:
        return "dynamic"

with open("param.py", "r") as f:
    global txt
    txt = f.read()
    for (cand_max, do_dyn_cand, cth, sel_size, do_dyn_sel, sth) in config:
        txt = sub("do_dynamic_candidate", do_dyn_cand)
        txt = sub("candidate_max_budget", cand_max)
        txt = sub("dynamic_candidate_threshold", cth)

        txt = sub("do_dynamic_selection", do_dyn_sel)
        txt = sub("selection_size", sel_size)
        txt = sub("dynamic_selection_threshold", sth)

        txt = sub("min_aware_iteration", cand_max)
        # print(txt)
        # print("-----------------------------------------------")

        # Characterization string
        char = dyn_or_static(do_dyn_cand, do_dyn_sel)
        char = "negative"
        with open("param.py", "wb") as fw:
            fw.write(txt)

        # Set filename from config
        filename = "bert_{}".format("_".join(
          [char, str(cand_max), str(sel_size), do_dyn_cand, do_dyn_sel, "sample1500", "new"]
        ).lower())

        # May requires customization casy by case
        command = "./start_squad.sh"
        os.system(" ".join([command, filename]))

        # Send messages to slack
        os.system("slackmessage \"`cat {}`\"".format(filename))
