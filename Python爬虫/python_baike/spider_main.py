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
                 print 'craw %d : %s' %(count,new_url)
                 #下载新的url页面
                 html_cont = self.downloader.download(new_url)
                 print 'download is ok'

                 #对下载的页面进行解析
                 #print html_cont
                 new_urls,new_data = self.parser.parse(new_url,html_cont)
                 print 'parser is ok'
                 #print new_data

                 #解析后得到新的url数据，并将新的url补充进url管理器
                 self.urls.add_new_urls(new_urls)
                 print 'add_new_urls is ok'

                 #进行数据的收集
                 self.outputer.collect_data(new_data)
                 print 'data_collect is ok'
                 
                 if count == 10:
                     break

                 count = count + 1
            except:
                print 'craw failed'
        #输出收集好的数据
            self.outputer.output_html()

if __name__=="__main__":
    root_url = "http://baike.baidu.com/item/python"
    obj_spider = SpiderMain()
    obj_spider.craw(root_url)
