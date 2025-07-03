import fort_myrmidon as m
import unittest


class ColorTestCase(unittest.TestCase):
    def test_have_a_default_palette(self):
        for i in range(2 * len(m.DefaultPalette())):
            self.assertEqual(
                m.DefaultPaletteColor(i),
                m.DefaultPalette()[i % len(m.DefaultPalette())],
            )

    def test_format_color(self):
        testdata = [
            (m.DefaultPaletteColor(0), "(230, 159, 0)"),
            (m.DefaultPaletteColor(1), "(86, 180, 233)"),
            (m.DefaultPaletteColor(2), "(0, 158, 115)"),
            (m.DefaultPaletteColor(3), "(240, 228, 66)"),
            (m.DefaultPaletteColor(4), "(0, 114, 178)"),
            (m.DefaultPaletteColor(5), "(213, 94, 0)"),
            (m.DefaultPaletteColor(6), "(204, 121, 167)"),
            (m.DefaultPaletteColor(7), "(230, 159, 0)"),
        ]
        for c, e in testdata:
            self.assertEqual(str(c), e)
