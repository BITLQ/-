#!/usr/bin/env python
# coding=utf-8
import urllib2

class HtmlDownloader(object):
    
    def download(seld,url):
        if url is None:
            return None
        respose =  urllib2.urlopen(url)

        if respose.getcode() != 200:
            return None

        return respose.read()

