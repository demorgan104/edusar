import copy
import importlib.util
import json
from pathlib import Path
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('generator', ROOT / 'tools/generate.py')
generator = importlib.util.module_from_spec(spec)
spec.loader.exec_module(generator)

class GeneratorTests(unittest.TestCase):
    def setUp(self):
        self.config = json.loads((ROOT / 'config/demo.json').read_text())

    def generate(self, config):
        with tempfile.TemporaryDirectory() as temp:
            source = Path(temp) / 'config.json'
            output = Path(temp) / 'config.c'
            source.write_text(json.dumps(config))
            generator.generate(source, output)
            return output.read_text()

    def test_multiple_signals(self):
        second = copy.deepcopy(self.config['signals'][0])
        second['name'] = 'Temperature'
        for key in ('signal_id', 'com_tx', 'com_rx', 'canif_tx', 'canif_rx', 'can_id'):
            second[key] += 100
        self.config['signals'].append(second)
        self.assertIn('signals, 2u', self.generate(self.config))
        self.assertEqual(self.generate(self.config), self.generate(self.config))

    def test_invalid_configurations(self):
        for key, value in [('can_id', 2048), ('signal_id', -1), ('com_tx', True), ('name', 'bad name')]:
            config = copy.deepcopy(self.config)
            config['signals'][0][key] = value
            with self.subTest(key=key), self.assertRaises(ValueError):
                self.generate(config)
        for signals in ([], self.config['signals'] * 2, self.config['signals'] * 17):
            with self.assertRaises(ValueError):
                self.generate({'signals': signals})

if __name__ == '__main__':
    unittest.main()
