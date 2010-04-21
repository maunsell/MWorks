function transformedData = transform_anscombe(inputData)
%TRANSFORM_ANSCOMBE (sacvector2): do a var-stabilizing Anscombe transformation
%   transformedData = TRANSFORM_ANSCOMBE(inputData)
%
%   Can be used by PREF_DIR_BOTH_SEQUENTIAL, DECODE_RATE_TO_VECTOR
%
%$Id: transform_anscombe.m 125 2008-03-20 20:19:22Z vincent $

transformedData = sqrt(inputData + (3/8));
