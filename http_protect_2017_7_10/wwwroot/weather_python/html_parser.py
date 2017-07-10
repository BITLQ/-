#!/usr/bin/env python
# coding=utf-8

from bs4 import BeautifulSoup
import re
import urlparse

class HtmlParser(object):
    
    def _get_new_urls(self,page_url,soup):
        new_urls = set()
       
       #陕西地区的url格式为：<a href="http://www.weather.com.cn/weather1d/101111001.shtml#around2"""
       
       # links = soup.find_all('a',href=re.compile(r"/view/\d*\.htm"))
        links = soup.find_all('a',href=re.compile(r"/1011101\d*.shtml#around2"))

        for link in links:
            new_url = link['href']
            new_full_url = urlparse.urljoin(page_url,new_url)
            new_urls.add(new_full_url)
            #new_urls.add(new_url)
        return new_urls

    
    def _get_new_data(self,page_url,soup):
        res_data = {}
        #url
        res_data['url'] = page_url

        #<div class="crumbs fl">""      
        title_node = soup.find('div',class_="crumbs fl")
        res_data['title'] = title_node.get_text()
        
        #<div class="today clearfix" id="today">""""
        summary_node = soup.find('div',class_='t')
        res_data['summary'] = summary_node.get_text()

        #<ul class="t clearfix">""
        #summary_node = soup.find('ul',class_='t clearfix')
        #res_data['summary'] = summary_node.get_text()
        
        #<div class="curve_livezs" id="curve">""""
        #summary_node = soup.find('div',class_='curve_livezs',id='curve')
        #res_data['summary'] = summary_node.get_text()

        return res_data

    def parse(self,page_url,html_count):
        if page_url is None or html_count is None:
            return

        soup = BeautifulSoup(html_count,'html.parser',from_encoding='utf-8')

        new_urls = self._get_new_urls(page_url,soup)

        new_data = self._get_new_data(page_url,soup)
        return new_urls,new_data
    
        
