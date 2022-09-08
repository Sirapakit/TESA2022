function [DH] = DhThis(theta,d,a,alpha)
% add DH parameter to transformation matrix
%to find the matrix assign length alpha and theta of each joint
DH=[cosd(theta), -sind(theta)*cosd(alpha), sind(theta)*sind(alpha), acosd(theta);
     sind(theta), cosd(theta)*cosd(alpha),  -cosd(theta)*sind(alpha), asind(theta);
     0,sind(alpha),cosd(alpha),d;
     0,0,0,1];
end
