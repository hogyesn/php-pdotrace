# pdotrace

A PHP extension for logging all PDO queries to a file, including bound parameters, callstacks, and execution times.

> ⚠️ **Development tool only** – Not intended for production environments.

## Features

- Log all PDO queries with SQL statements
- Capture query execution times
- Full callstack traces
- Bound parameter tracking (coming soon)
- File-based logging (JSON format)

## Build & Debug

This project uses [PED (PHP Extension Development)](https://hub.docker.com/r/hogyesn/php-extension-dev) environment.

```bash
# Start the development container
docker compose up -d

# Build the extension
./ped build pdotrace

# Debug/test the extension
./ped debug pdotrace
```

## Configuration

`php.ini`:

```ini
extension=pdotrace.so
pdotrace.enable=1
pdotrace.log_file_path=/tmp/pdotrace.log
```

## Output Example

```json
{
  "trace_id": "3f828f8033c620fa3855dec3d9545cf8",
  "endpoint": "/api/users/42",
  "timestamp": 1738764800,
  "runtime": 0.002,
  "query": "SELECT * FROM users WHERE id = :id AND status = :status",
  "params": {
    ":id": 42,
    ":status": "active"
  },
  "callstack": [
    {
      "function_name": "execute",
      "class_name": "PDOStatement",
      "file": "/var/www/app/Repository/UserRepository.php",
      "line": 45
    },
    {
      "function_name": "findById",
      "class_name": "UserRepository",
      "file": "/var/www/app/Controller/UserController.php",
      "line": 23
    }
  ]
}
```

## Roadmap

### Short-term
- [ ] Fix memory leaks
- [ ] Handle bound params via `stmt_execute` hook
- [ ] Detect CLI/request entrypoint name (path + params)

### Future
- [ ] HTTP logging support (for PDOTrace UI)
- [ ] Publish to [PIE](https://github.com/php/pie) for easy installation (`pie install hogyesn/pdotrace`)
