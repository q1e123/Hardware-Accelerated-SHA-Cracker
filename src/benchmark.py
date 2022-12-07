import time

class Benchmarker():
    def benchmark(self, function, args):
        start_time = time.time()
        result = function(*args)
        end_time = time.time()
        duration = end_time - start_time
        return result, duration
