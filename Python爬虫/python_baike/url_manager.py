#!/usr/bin/env python
# coding=utf-8


class UrlManager(object):
    def __init__(self):
        self.new_urls = set()
        self.old_urls = set()

    def add_new_url(self,url):
        if url is None:
            return
        if url not in self.new_urls and url not in self.old_urls:
            self.new_urls.add(url)
            print 'add_new_url is ok'

    def add_new_urls(self,urls):
        if urls is None or len(urls) == 0:
            print 'urls is None'
            return
        for url in urls:
            print url
            self.add_new_url(url)

    
    def has_new_url(self):
        print len(self.new_urls)
        return len(self.new_urls) != 0
    
    def get_new_url(self):
        new_url = self.new_urls.pop()
        self.old_urls.add(new_url)
        return new_url
