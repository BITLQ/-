#!/usr/bin/env python
# coding=utf-8

import url_manager,html_downloader,html_parser,html_outputer

class SpiderMain(object):
    def __init__(self):
        #url管理器
        self.urls = url_manager.UrlManager()
        #url下载器
        self.downloader = html_downloader.HtmlDownloader()
        #url解释器
        self.parser = html_parser.HtmlParser()
        #url输出器
        self.outputer = html_outputer.HtmlOutputer()


    #爬虫的循环函数
    def craw(self,root_url):
        count = 1
        #添加开始的url
        self.urls.add_new_url(root_url)
        #如果还有新的url进入循环
        while self.urls.has_new_url():
            #获取新的url
            try:
                 new_url = self.urls.get_new_url()
                 #下载新的url页面
                 html_cont = self.downloader.download(new_url)

                 #对下载的页面进行解析
                 new_urls,new_data = self.parser.parse(new_url,html_cont)
                 
                 #解析后得到新的url数据，并将新的url补充进url管理器
                 self.urls.add_new_urls(new_urls)

                 #进行数据的收集
                 #print new_data['title']
                 #print new_data['summary']
                 self.outputer.collect_data(new_data)
                 
                 if count == 100:
                     break

                 count = count + 1
            except:
                print 'craw failed'
        #输出收集好的数据
            self.outputer.output_html()

if __name__=="__main__":
    #root_url = "http://www.weather.com.cn/weather/101110101.shtml"
    root_url = "http://www.weather.com.cn/weather1d/101110101.shtml#around2"
    obj_spider = SpiderMain()
    obj_spider.craw(root_url)
