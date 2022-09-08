%%open file
edit robotCtrlr.sfx
%%
%call stateflow and show initiation
robot = robotCtrlr()
rb2 = TryStateflowFull()
%%
%call HOME event
robot.HOME
%%
%call START event
robot.START

edit trysf.sfx