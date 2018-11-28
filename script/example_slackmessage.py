import re
import os

command = []
                # cand_max / do_dyn_cand. /  thresh / sel_size / do_dyn_sel /  thresh
# command.append([120,          "False",          12.8,    60,        "False",     12.8])
# command.append([320,          "False",          12.8,    10,        "False",     12.8])
command.append([320,          "False",          12.8,    "None",    "True",      12.8])

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
    for (cand_max, do_dyn_cand, cth, sel_size, do_dyn_sel, sth) in command:
        txt = sub("do_dynamic_candidate", do_dyn_cand)
        txt = sub("candidate_max_budget", cand_max)
        txt = sub("dynamic_candidate_threshold", cth)

        txt = sub("do_dynamic_selection", do_dyn_sel)
        txt = sub("selection_size", sel_size)
        txt = sub("dynamic_selection_threshold", sth)
        # print(txt)
        # print("-----------------------------------------------")
        char = dyn_or_static(do_dyn_cand, do_dyn_sel)
        char = "light_weight"
        with open("param.py", "wb") as fw:
            fw.write(txt)

        filename = "log_{}".format("_".join(
          [char, str(cand_max), str(sel_size), do_dyn_cand, do_dyn_sel, "sample1500", "new"]
        ).lower())

        command = "./start_squad.sh"

        os.system(" ".join([command, filename]))

    os.system("slackmessage `cat {}`".format(filename))
