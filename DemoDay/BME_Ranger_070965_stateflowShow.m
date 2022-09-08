%%open file
edit robotCtrlr.sfx
%%
%call stateflow and show initiation
robot = robotCtrlr()
%%
%call HOME event
robot.HOME
%%
%call START event
robot.START
