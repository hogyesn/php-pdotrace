<?php

// Debug script: pdo_all.php
// Project: pdotrace
// Created: Fri Jan 16 13:35:05 UTC 2026

class TestPDOAllQueries {
	public function run() {
		$pdo = new PDO('sqlite::memory:');
		$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		// setup
		$pdo->exec('CREATE TABLE t (id INTEGER, val TEXT)');
		$pdo->exec("INSERT INTO t VALUES (1, 'a'), (2, 'b')");

		/** 1. PDO::query() */
		$pdo->query('SELECT * FROM t');

		/** 2. PDO::exec() */
		$pdo->exec("UPDATE t SET val = 'c' WHERE id = 1");

		/** 3. PDO::prepare() + execute() (no params) */
		$stmt = $pdo->prepare('SELECT * FROM t');
		$stmt->execute();

		/** 4. prepare + execute(array params) */
		$stmt = $pdo->prepare('SELECT * FROM t WHERE id = ?');
		$stmt->execute([1]);

		/** 5. prepare + bindValue */
		$stmt = $pdo->prepare('SELECT * FROM t WHERE id = :id');
		$stmt->bindValue(':id', 2, PDO::PARAM_INT);
		$stmt->execute();

		/** 6. prepare + bindParam (by reference) */
		$stmt = $pdo->prepare('SELECT * FROM t WHERE id = :id');
		$id = 1;
		$stmt->bindParam(':id', $id, PDO::PARAM_INT);
		$stmt->execute();

		/** 7. PDOStatement::execute() multiple times */
		$stmt = $pdo->prepare('SELECT * FROM t WHERE id = ?');
		$stmt->execute([1]);
		$stmt->execute([2]);

		/** 8. transaction + exec/query */
		$pdo->beginTransaction();
		$pdo->exec("INSERT INTO t VALUES (3, 'd')");
		$pdo->query('SELECT * FROM t');
		$pdo->commit();

		/** 9. implicit statement via fetch helpers */
		$pdo->query('SELECT * FROM t')->fetch();
		$pdo->query('SELECT * FROM t')->fetchAll();

		/** 10. prepared statement + fetch */
		$stmt = $pdo->prepare('SELECT * FROM t');
		$stmt->execute();
		$stmt->fetch();
	}
}

(new TestPDOAllQueries())->run();
