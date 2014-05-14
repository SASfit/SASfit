import xml.sax

import unittest
import test_utils

import xmlreader

class XmlReaderTestCase(unittest.TestCase):
    def test_XmlDumpAllRevs(self):
        pages = [r for r in xmlreader.XmlDump("data/article-pear.xml", allrevisions=True).parse()]
        self.assertEquals(4, len(pages))
        self.assertEquals(u"Automated conversion", pages[0].comment)
        self.assertEquals(u"Pear", pages[0].title)
        self.assertEquals(u"24278", pages[0].id)
        self.assertTrue(pages[0].text.startswith('Pears are [[tree]]s of'))
        self.assertEquals(u"Quercusrobur", pages[1].username)
        self.assertEquals(u"Pear", pages[0].title)

    def test_XmlDumpFirstRev(self):
        pages = [r for r in xmlreader.XmlDump("data/article-pear.xml").parse()]
        self.assertEquals(1, len(pages))
        self.assertEquals(u"Automated conversion", pages[0].comment)
        self.assertEquals(u"Pear", pages[0].title)
        self.assertEquals(u"24278", pages[0].id)
        self.assertTrue(pages[0].text.startswith('Pears are [[tree]]s of'))
        self.assertTrue(not pages[0].isredirect)

    def test_XmlDumpRedirect(self):
        pages = [r for r in xmlreader.XmlDump("data/article-pyrus.xml").parse()]
        self.assertTrue(pages[0].isredirect)

    def test_MediaWikiXmlHandler(self):
        handler = xmlreader.MediaWikiXmlHandler()
        pages = []
        def pageDone(page):
            pages.append(page)
        handler.setCallback(pageDone)
        xml.sax.parse("data/article-pear.xml", handler)
        self.assertEquals(u"Pear", pages[0].title)
        self.assertEquals(4, len(pages))
        self.assertNotEquals("", pages[0].comment)

if __name__ == '__main__':
    unittest.main()
