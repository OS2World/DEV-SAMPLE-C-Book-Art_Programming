APP=MLE3

$(APP).EXE:                     $(APP).OBJ \
                                $(APP).RES
        LINK386 /A:16 $(APP),$(APP),NUL,OS2386,$(APP);
        RC $(APP).RES $(APP).EXE

$(APP).RES:                     $(APP).RC \
                                $(APP)RC.H
        RC -r $(APP).RC $(APP).RES

$(APP).OBJ:                     $(APP).C \
                                $(APP)RC.H
        ICC -C+ -Kb+ -Ss+ $(APP).C

