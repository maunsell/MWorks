function code = codec_tag2code(codec, tagnames)
%CODEC_TAG2CODE (mw-box): given a codec and variable names, return codes
%
%$Id: codec_tag2code.m 54 2010-01-15 16:06:48Z histed $

%code = codec(codec_tag2idx(codec,tagname)).code;
ns = codec_tag2idx(codec, tagnames);
codeList = [codec.code];
code = codeList(ns);

