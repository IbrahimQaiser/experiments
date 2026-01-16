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

3. Database amplification can be prevented using `dataloader`, the library will batch calls to the database. The function must take a list of keys as argument, and it must return a list of the same size. If an error occurs while fetching a single key, it should return null / new Error instead.

Now, Instead of ~3000 database hits, it's reduced to around 4 SQL queries.

A new dataloader object must be created for every client so that cache is not accidently shared between different clients (unless it doesn't matter in that use case)

