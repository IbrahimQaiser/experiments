# Commands

Start postgres:

```
docker run --name pg -e POSTGRES_PASSWORD=dev -p 0.0.0.0:5432:5432 -v pg_data:/var/lib/postgresql/data postgres:16-alpine
```

Access postgres shell:

```
docker exec -it pg psql -U postgres
```

Run initialization script:

```
docker exec -i pg psql -U postgres < init.sql
```

Start postgraphile:

```
npx postgraphile \
    --enhance-graphiql \
    --watch \
    -c "postgres://postgres:dev@0.0.0.0:5432/postgres?sslmode=disable" \
    --schema app_public \
    --append-plugins @graphile-contrib/pg-simplify-inflector
```
