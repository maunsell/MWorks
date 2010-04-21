function origData = transfinv_anscombe(inputTransformedData)
%TRANSFINV_ANSCOMBE (sacvector2): invert the var-stabilizing Anscombe transf
%   origData = TRANSFINV_ANSCOMBE(inputTransformedData)
%
%   Can be used by PREF_DIR_BOTH_SEQUENTIAL, DECODE_RATE_TO_VECTOR
%
%$Id: transfinv_anscombe.m 125 2008-03-20 20:19:22Z vincent $

origData = sign(inputTransformedData) ...
           .* ((inputTransformedData.^2) - (3/8));

