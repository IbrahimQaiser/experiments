# Graphql programs

## Findings

1. Aliases

```
{ products { name id idCPy: id }  }
```
Graphql does not cache the results of aliases, and hence the (trivial resolver for id) is called twice, once for id and once for idCpy
So a malicious user can create an attack by creating multiple aliases of the same field. 
Even though the underlying data is the same, GraphQL fetches it multiple times.

The trivial resolver (for id) is called even after a mutation
```
mutation {
    addProduct(product: {name: "Laptop"}) {
        name
        id
    }
}
```
This is because the mutation returns data, and that returned data is resolved like a query

2. If the server is implemented in a naive fashion, like in `02-graphql-with-postgres/`, it can result in a `N+1` explosion, and can also lead to DoS attacks. GraphQL requires good sandboxing to make it usable

One query, results in ~3000 SQL queries
```
{
  users {
    name
    id
    posts {
      title
      author {
        name
      }
      comments {
        body
        author {
          name
        }
      }
    }
  }
}

[2026-01-15T21:32:48.157Z] POST /graphql - IP: ::1 - Status: 200 - 507ms
SQL Queries - Users: 1, Posts: 10, Comments: 1009, User: 2009, Total: 3029
Resolve calls - Users: 1, Posts: 10, PostAuthor: 1009, PostComments: 1009, CommentAuthor: 1000, Total: 3029
```

This happens because the server executes the query as a GraphQL program, so it does not have an intelligent planner like a SQL execution engine. 

So think of a GraphQL query as a user supplied program, so it should be treated with caution, and should not be blindly executed

- Aliases bypass caching of fields
- Fragments increase the amount of work required
- Recursive graph walks are unbounded, so a recursion limit needs to be applied
- Cost is not dependent upon the resource requested or the endpoint, it depends upon the shape of the query

3. Database amplification can be prevented using `dataloader`, the library will batch calls to the database. The function must take a list of keys as argument, and it must return a list of the same size. If an error occurs while fetching a single key, it should return null / new Error instead. The SQL query returns a flat list, hence it should be grouped by the key (either in SQL or in JS), so that an array of same size can be returned

Now, Instead of ~3000 database hits, it's reduced to around 4 SQL queries.
```
[2026-01-16T17:55:31.936Z] POST /graphql - IP: ::1 - Status: 200 - 122ms
SQL Queries - Users: 1, Posts: 1, Comments: 1, User: 1, Total: 4
Resolve calls - Users: 1, Posts: 10, PostAuthor: 1010, PostComments: 1010, CommentAuthor: 1000, Total: 3031
```

A new dataloader object must be created for every client so that cache is not accidently shared between different clients (unless it doesn't matter in that use case)

Guardrails to prevent GraphQL abuse

1) Set maximum query depth limit, this limits the nesting a query can have, thereby reducing the chance of highly nested queries crashing the server. The package `graphql-depth-limit` adds depth limiting to the query, and it also handles the case of fragments, i.e. if the number of levels of top level query + levels inside fragment exceed the set depth, an error is returned.

2) Set a timeout so that queries that take too long get cancelled, (Note: The solution implemented here does not work correctly since `graphql-js` does not support timeouts, if supported, use `AbortController`)

Three types of timeout can be set:

- Request level timeout (after each middleware, so if a middleware takes longer than `x seconds`, stop execution)
- Database level timeout, whenever a database fetch takes longer than the set limit, timeout the request
- Graphql resolver timeout, set it to timeout if the graph resolving takes longer than the set time

3) Query complexity analysis, since traditional rate limiting software does not work, the application has to implement heuristics to identify the cost with executing a query. If the cost of execution exceeds the set threshold, the query must be dropped. Traditional ratelimiting does not work since only a single query is sent, but that query can involve a lot of work.

In the cost based solution, we set a cost per field, and then check the AST to find out the total cost for the query
