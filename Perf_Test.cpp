// Perf_Test.cpp : Defines the entry point for the application.
//

#include <benchmark/benchmark.h>
#include <memory>

namespace in {
	struct A {
		int x = 0;
		virtual ~A() = 0;
		virtual void f() = 0;
	};

	inline A::~A() { }

	struct B : A {
		~B() final {};
		void f() final {
			x += 2;
		}
	};

	struct C : A {
		~C() final {};
		void f() final {
			x += 1;
		}
	};
}

static void inheritance_vec(benchmark::State& state) {
	using namespace in;
	std::vector<A*> as;
	std::vector<B> bs;
	std::vector<C> cs;
	bs.resize(state.range(0) / 2);
	cs.resize(state.range(0) / 2);

	for (auto& x : bs) as.push_back(&x);
	for (auto& x : cs) as.push_back(&x);

	for (auto _ : state) {
		for (auto& x : as) {
			x->f();
			benchmark::DoNotOptimize(x->x);
			benchmark::ClobberMemory();
		}
	}
}

static void inheritance_vec_rand(benchmark::State& state) {
	using namespace in;
	std::vector<A*> as;
	std::vector<B> bs;
	std::vector<C> cs;
	bs.resize(state.range(0) / 2);
	cs.resize(state.range(0) / 2);

	size_t b_i = 0;
	size_t c_i = 0;

	for (size_t i = 0; i < state.range(0); ++i) {
		if (b_i >= bs.size()) {
			as.push_back(&cs[c_i++]);
		} else if (c_i >= cs.size()) {
			as.push_back(&bs[b_i++]);
		} else {
			if (rand() % 2) {
				as.push_back(&bs[b_i++]);
			} else {
				as.push_back(&cs[c_i++]);
			}
		}
	}

	for (auto _ : state) {
		for (auto& x : as) {
			x->f();
			benchmark::DoNotOptimize(x->x);
			benchmark::ClobberMemory();
		}
	}
}

static void inheritance_seq(benchmark::State& state) {
	using namespace in;
	std::vector<std::unique_ptr<A>> as;

	for (size_t i = 0; i < state.range(0) / 2; ++i) as.push_back(std::make_unique<B>());
	for (size_t i = 0; i < state.range(0) / 2; ++i) as.push_back(std::make_unique<C>());

	for (auto _ : state) {
		for (auto& x : as) {
			x->f();
			benchmark::DoNotOptimize(x->x);
			benchmark::ClobberMemory();
		}
	}
}
static void inheritance_rand(benchmark::State& state) {
	using namespace in;
	std::vector<std::unique_ptr<A>> as;

	for (size_t i = 0; i < state.range(0); ++i) {
		if ((rand() % 2) == 0) as.push_back(std::make_unique<B>());
		else                   as.push_back(std::make_unique<C>());
	}

	for (auto _ : state) {
		for (auto& x : as) {
			x->f();
			benchmark::DoNotOptimize(x->x);
			benchmark::ClobberMemory();
		}
	}
}
// Register the function as a benchmark
BENCHMARK(inheritance_vec)->Range(8, 8 << 15)->Unit(benchmark::kMicrosecond);
BENCHMARK(inheritance_vec_rand)->Range(8, 8 << 15)->Unit(benchmark::kMicrosecond);
BENCHMARK(inheritance_seq)->Range(8, 8 << 15)->Unit(benchmark::kMicrosecond);
BENCHMARK(inheritance_rand)->Range(8, 8 << 15)->Unit(benchmark::kMicrosecond);

namespace p {
	struct B {
		int x;
		void f() {
			x += 2;
		}
	};

	struct C {
		int x;
		void f() {
			x += 1;
		}
	};
}

// Define another benchmark
static void pod(benchmark::State& state) {

	using namespace p;

	std::vector<B> bs;
	std::vector<C> cs;

	for (size_t i = 0; i < state.range(0) / 2; ++i) {
		bs.push_back({});
	}
	for (size_t i = 0; i < state.range(0) / 2; ++i) {
		cs.push_back({});
	}

	auto iter = [](auto& v) {
		for (auto& x : v) {
			x.f();
			benchmark::DoNotOptimize(x.x);
			benchmark::ClobberMemory();
		}
	};

	for (auto _ : state) {
		iter(bs);
		iter(cs);
	}

}
BENCHMARK(pod)->Range(8, 8 << 15)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
