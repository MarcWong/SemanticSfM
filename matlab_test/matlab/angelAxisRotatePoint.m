function result = angelAxisRotatePoint(angle_axis, pt)
    epsilon = realmin('double');
    theta2 = angle_axis(1) *  angle_axis(1) + angle_axis(2) * angle_axis(2) + angle_axis(3) * angle_axis(3);
    if  theta2 > epsilon
        theta = sqrt(theta2);
        costheta = cos(theta);
        sintheta = sin(theta);
        theta_inverse = 1.0 / theta;
        w = theta_inverse .* angle_axis;
        w_cross_pt = [w(2) * pt(3) -  w(3) * pt(2), w(3) * pt(1) -  w(1) * pt(3), w(1) * pt(2) -  w(2) * pt(1)];
        tmp = (w(1)*pt(1) + w(2)*pt(2) + w(3)*pt(3)) * (1 - costheta);
        result = pt .* costheta + w_cross_pt.*sintheta + w.*tmp;
    else
        w_cross_pt = [angel_axis(2) * pt(3) -  angel_axis(3) * pt(2), angel_axis(3) * pt(1) -  angel_axis(1) * pt(3), angel_axis(1) * pt(2) -  angel_axis(2) * pt(1)];
        result = pt .*  w_cross_pt;
    end
end