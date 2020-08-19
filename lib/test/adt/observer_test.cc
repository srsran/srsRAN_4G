/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/adt/observer.h"
#include "srslte/common/test_common.h"

static_assert(srslte::is_observer<srslte::observer<int> >::value, "is_observer<> meta-function failed");
static_assert(not srslte::is_observer<std::function<void(int)> >::value, "is_observer<> meta-function failed");

struct M {
  M() = default;
  explicit M(int v) : val(v) {}
  M(M&&) noexcept = default;
  M(const M&)     = delete;
  M& operator=(M&&) noexcept = default;
  M& operator=(const M&) = delete;

  M& operator+=(int i)
  {
    val += i;
    return *this;
  }

  int val = 0;
};

struct lval_observer_tester {
  void trigger(M v_) { v = std::move(v_); }
  void foo(M v_)
  {
    v = std::move(v_);
    v += 1;
  }
  M v;
};
struct cref_observer_tester {
  void trigger(const M& v_) { v.val = v_.val; }
  void foo(const M& v_) { v.val = v_.val + 1; }
  M    v;
};
struct lref_observer_tester {
  void trigger(M& v_) { v.val = v_.val; }
  void foo(M& v_) { v.val = v_.val + 1; }
  M    v;
};
struct rref_observer_tester {
  void trigger(M&& v_) { v = std::move(v_); }
  void foo(M&& v_)
  {
    v = std::move(v_);
    v += 1;
  }
  M v;
};

int observable_test()
{
  // TEST l-value arguments passed by value
  {
    M                     val;
    srslte::observable<M> subject;
    TESTASSERT(subject.nof_observers() == 0);

    srslte::observer_id id1 = subject.subscribe([&val](M v) { val = std::move(v); });

    lval_observer_tester observer{}, observer2{};
    srslte::observer_id  id2 = subject.subscribe(observer);
    srslte::observer_id  id3 = subject.subscribe(observer2, &lval_observer_tester::foo);

    TESTASSERT(subject.nof_observers() == 3);
    TESTASSERT(val.val == 0);
    subject.dispatch(M{5});
    TESTASSERT(val.val == 5);
    TESTASSERT(observer.v.val == 5);
    TESTASSERT(observer2.v.val == 6);

    subject.unsubscribe(id1);
    TESTASSERT(subject.nof_observers() == 2);
    subject.unsubscribe(id2);
    TESTASSERT(subject.nof_observers() == 1);
    subject.unsubscribe(id3);
    TESTASSERT(subject.nof_observers() == 0);
  }

  // Test l-value arguments passed by const ref
  {
    M                            val;
    srslte::observable<const M&> subject;
    TESTASSERT(subject.nof_observers() == 0);

    subject.subscribe([&val](const M& v) { val.val = v.val; });

    cref_observer_tester observer{}, observer2{};
    subject.subscribe(observer);
    subject.subscribe(observer2, &cref_observer_tester::foo);

    M new_val{6};
    subject.dispatch(new_val);
    TESTASSERT(val.val == 6);
    TESTASSERT(observer.v.val == 6);
    TESTASSERT(observer2.v.val == 7);
  }

  // Test l-value arguments passed by ref
  {
    M                      val;
    srslte::observable<M&> subject;
    TESTASSERT(subject.nof_observers() == 0);

    subject.subscribe([&val](M& v) { val = std::move(v); });

    lref_observer_tester observer{}, observer2{};
    subject.subscribe(observer);
    subject.subscribe(observer2, &lref_observer_tester::foo);

    M new_val{6};
    subject.dispatch(new_val);
    TESTASSERT(val.val == 6);
    TESTASSERT(observer.v.val == 6);
    TESTASSERT(observer2.v.val == 7);
  }

  // Test r-value arguments
  {
    M                       val;
    srslte::observable<M&&> subject;
    TESTASSERT(subject.nof_observers() == 0);

    srslte::observer_id id1 = subject.subscribe([&val](M&& v) { val = std::move(v); });

    rref_observer_tester observer{}, observer2{};
    srslte::observer_id  id2 = subject.subscribe(observer);
    srslte::observer_id  id3 = subject.subscribe(observer2, &rref_observer_tester::foo);

    subject.dispatch(M{3});
    TESTASSERT(val.val == 3);
    TESTASSERT(observer.v.val == 3);
    TESTASSERT(observer2.v.val == 4);

    subject.unsubscribe(id1);
    subject.unsubscribe(id2);
    subject.unsubscribe(id3);
    TESTASSERT(subject.nof_observers() == 0);
  }

  return SRSLTE_SUCCESS;
}

int event_dispatcher_test()
{
  srslte::event_dispatcher<M> signaller;

  M val;
  signaller.subscribe([&val](const M& ev) { val.val = ev.val; });

  cref_observer_tester observer, observer2;
  signaller.subscribe(observer);
  signaller.subscribe(observer2, &cref_observer_tester::foo);

  TESTASSERT(val.val == 0);
  TESTASSERT(observer.v.val == 0);
  TESTASSERT(observer2.v.val == 0);
  signaller.dispatch(M{2});
  TESTASSERT(val.val == 2);
  TESTASSERT(observer.v.val == 2);
  TESTASSERT(observer2.v.val == 3);

  val.val         = 1;
  observer.v.val  = 0;
  observer2.v.val = 5;
  signaller.dispatch(M{2});
  TESTASSERT(val.val == 2);
  TESTASSERT(observer.v.val == 2);
  TESTASSERT(observer2.v.val == 3);

  return SRSLTE_SUCCESS;
}

int event_queue_test()
{
  srslte::event_queue<M> signaller;

  M val;
  signaller.subscribe([&val](const M& ev) { val.val = ev.val; });
  cref_observer_tester observer, observer2;
  signaller.subscribe(observer);
  signaller.subscribe(observer2, &cref_observer_tester::foo);

  TESTASSERT(val.val == 0);
  TESTASSERT(observer.v.val == 0);
  TESTASSERT(observer2.v.val == 0);
  signaller.enqueue(M{2});
  TESTASSERT(val.val == 0);
  TESTASSERT(observer.v.val == 0);
  TESTASSERT(observer2.v.val == 0);
  signaller.process();
  TESTASSERT(val.val == 2);
  TESTASSERT(observer.v.val == 2);
  TESTASSERT(observer2.v.val == 3);

  return SRSLTE_SUCCESS;
}

int unique_subscribe_test()
{
  {
    srslte::event_dispatcher<M> signaller;
    cref_observer_tester        observer;
    TESTASSERT(signaller.nof_observers() == 0);
    {
      srslte::unique_observer_id<M> obs{signaller, observer};
      TESTASSERT(signaller.nof_observers() == 1);
    }
    TESTASSERT(signaller.nof_observers() == 0);
  }

  {
    srslte::event_queue<M> signaller;
    cref_observer_tester   observer;
    TESTASSERT(signaller.nof_observers() == 0);
    {
      srslte::unique_observer_id<M> obs{signaller, observer};
      TESTASSERT(signaller.nof_observers() == 1);
    }
    TESTASSERT(signaller.nof_observers() == 0);
  }

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(observable_test() == SRSLTE_SUCCESS);
  TESTASSERT(event_dispatcher_test() == SRSLTE_SUCCESS);
  TESTASSERT(event_queue_test() == SRSLTE_SUCCESS);
  TESTASSERT(unique_subscribe_test() == SRSLTE_SUCCESS);

  printf("Success\n");

  return 0;
}
