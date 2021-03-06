.. _mod-stats:

Statistics collector
--------------------

This modules gathers various counters from the query resolution and server internals,
and offers them as a key-value storage. Any module may update the metrics or simply hook
in new ones.

.. code-block:: none

	-- Enumerate metrics
	> stats.list()
	[answer.cached] => 486178
	[iterator.tcp] => 490
	[answer.noerror] => 507367
	[answer.total] => 618631
	[iterator.udp] => 102408
	[query.concurrent] => 149

	-- Query metrics by prefix
	> stats.list('iter')
	[iterator.udp] => 105104
	[iterator.tcp] => 490

	-- Set custom metrics from modules
	> stats['filter.match'] = 5
	> stats['filter.match']
	5

	-- Fetch most common queries
	> stats.frequent()
	[1] => {
		[type] => 2
		[count] => 4
		[name] => cz.
	}

	-- Fetch most common queries (sorted by frequency)
	> table.sort(stats.frequent(), function (a, b) return a.count > b.count end)

	-- Show recently contacted authoritative servers
	> stats.upstreams()
	[2a01:618:404::1] => {
	    [1] => 26 -- RTT
	}
	[128.241.220.33] => {
	    [1] => 31 - RTT
	}

Properties
^^^^^^^^^^

.. function:: stats.get(key)

  :param string key: i.e. ``"answer.total"``
  :return: ``number``

Return nominal value of given metric. 

.. function:: stats.set(key, val)

  :param string key:  i.e. ``"answer.total"``
  :param number val:  i.e. ``5``

Set nominal value of given metric.

.. function:: stats.list([prefix])

  :param string prefix:  optional metric prefix, i.e. ``"answer"`` shows only metrics beginning with "answer"

Outputs collected metrics as a JSON dictionary.

.. function:: stats.upstreams()

Outputs a list of recent upstreams and their RTT. It is sorted by time and stored in a ring buffer of
a fixed size. This means it's not aggregated and readable by multiple consumers, but also that
you may lose entries if you don't read quickly enough. The default ring size is 512 entries, and may be overriden on compile time by ``-DUPSTREAMS_COUNT=X``.

.. function:: stats.frequent()

Outputs list of most frequent iterative queries as a JSON array. The queries are sampled probabilistically,
and include subrequests. The list maximum size is 5000 entries, make diffs if you want to track it over time.

.. function:: stats.clear_frequent()

Clear the list of most frequent iterative queries.


Built-in statistics
^^^^^^^^^^^^^^^^^^^

Built-in counters keep track of number of queries and answers matching specific criteria.

+----------------------------------------------------+
| **Global answer counters**                         |
+-----------------+----------------------------------+
| answer.total    | total number of answered queries |
+-----------------+----------------------------------+
| answer.cached   | queries answered from cache      |
+-----------------+----------------------------------+

+-----------------+----------------------------------+
| **Answers categorized by RCODE**                   |
+-----------------+----------------------------------+
| answer.noerror  | NOERROR answers                  |
+-----------------+----------------------------------+
| answer.nodata   | NOERROR, but empty answers       |
+-----------------+----------------------------------+
| answer.nxdomain | NXDOMAIN answers                 |
+-----------------+----------------------------------+
| answer.servfail | SERVFAIL answers                 |
+-----------------+----------------------------------+

+-----------------+----------------------------------+
| **Answer latency**                                 |
+-----------------+----------------------------------+
| answer.1ms      | completed in 1ms                 |
+-----------------+----------------------------------+
| answer.10ms     | completed in 10ms                |
+-----------------+----------------------------------+
| answer.50ms     | completed in 50ms                |
+-----------------+----------------------------------+
| answer.100ms    | completed in 100ms               |
+-----------------+----------------------------------+
| answer.250ms    | completed in 250ms               |
+-----------------+----------------------------------+
| answer.500ms    | completed in 500ms               |
+-----------------+----------------------------------+
| answer.1000ms   | completed in 1000ms              |
+-----------------+----------------------------------+
| answer.1500ms   | completed in 1500ms              |
+-----------------+----------------------------------+
| answer.slow     | completed in more than 1500ms    |
+-----------------+----------------------------------+

+-----------------+----------------------------------+
| **Answer flags**                                   |
+-----------------+----------------------------------+
| answer.aa       | authoritative answer             |
+-----------------+----------------------------------+
| answer.tc       | truncated answer                 |
+-----------------+----------------------------------+
| answer.ra       | recursion available              |
+-----------------+----------------------------------+
| answer.rd       | recursion desired (in answer!)   |
+-----------------+----------------------------------+
| answer.ad       | authentic data (DNSSEC)          |
+-----------------+----------------------------------+
| answer.cd       | checking disabled (DNSSEC)       |
+-----------------+----------------------------------+
| answer.do       | DNSSEC answer OK                 |
+-----------------+----------------------------------+
| answer.edns0    | EDNS0 present                    |
+-----------------+----------------------------------+

+-----------------+----------------------------------+
| **Query flags**                                    |
+-----------------+----------------------------------+
| query.edns      | queries with EDNS present        |
+-----------------+----------------------------------+
| query.dnssec    | queries with DNSSEC DO=1         |
+-----------------+----------------------------------+
