// Program to demonstrate dataloader library for batching and caching

import { graphql, GraphQLID, GraphQLList, GraphQLNonNull, GraphQLObjectType, GraphQLSchema, GraphQLString } from "graphql";
import express from 'express'
import sql, { getCommentsByPostBatch, getCommentsByPostCalls, getPostsByUserIdBatch, getPostsByUserIdCalls, getUserByIdBatch, getUserByIdCalls, getUsers, getUsersCalls } from "./db.js";
import { createHandler } from 'graphql-http/lib/use/express';
import { ruruHTML } from "ruru/server";
import DataLoader from "dataloader";
import depthLimit from "graphql-depth-limit";
import { documents } from "./documents.js";
import bodyParser from "body-parser";
const maxQueryDepth = 6

console.log('connected to database')
console.log("Documents: ", Object.keys(documents).map(x => { return { name: documents[x].name, hash: documents[x].hash } }))

// GraphQL schemas and types
let resolveCallCounts = {
    users: 0,
    posts: 0,
    postAuthor: 0,
    postComments: 0,
    commentAuthor: 0
};

const UserType = new GraphQLObjectType({
    name: 'User',
    fields: () => ({
        id: { type: new GraphQLNonNull(GraphQLID) },
        name: { type: new GraphQLNonNull(GraphQLString) },
        posts: {
            type: new GraphQLNonNull(new GraphQLList(new GraphQLNonNull(PostType))),
            resolve: async (user, _, context) => {
                resolveCallCounts.posts++;
                console.log('resolving posts for user', user.id, '(', user.name, ')')
                return await context.loaders.getPostsByUserId.load(user.id)
            }
        }
    })
})

const PostType = new GraphQLObjectType({
    name: 'Post',
    fields: () => ({
        id: { type: new GraphQLNonNull(GraphQLID) },
        title: { type: new GraphQLNonNull(GraphQLString) },
        body: { type: new GraphQLNonNull(GraphQLString) },
        author: {
            type: new GraphQLNonNull(UserType),
            resolve: async (post, _, context) => {
                resolveCallCounts.postAuthor++;
                console.log('resolving author for post', post.id)
                return await context.loaders.getUserById.load(post.user_id)
            }
        },
        comments: {
            type: new GraphQLNonNull(new GraphQLList(new GraphQLNonNull(CommentType))),
            resolve: async (post, _, context) => {
                resolveCallCounts.postComments++;
                console.log('resolving comments for post', post.id)
                return await context.loaders.getCommentsByPost.load(post.id)
            }
        }
    })
})

const CommentType = new GraphQLObjectType({
    name: 'Comment',
    fields: {
        id: { type: new GraphQLNonNull(GraphQLID) },
        body: { type: new GraphQLNonNull(GraphQLString) },
        author: {
            type: new GraphQLNonNull(UserType),
            resolve: async (comment, _, context) => {
                resolveCallCounts.commentAuthor++;
                console.log('resolving author for comment', comment.id)
                return await context.loaders.getUserById.load(comment.user_id)
            }
        }
    }
})

const schema = new GraphQLSchema({
    query: new GraphQLObjectType({
        name: 'Query',
        fields: {
            users: {
                type: new GraphQLNonNull(new GraphQLList(UserType)),
                resolve: async () => {
                    resolveCallCounts.users++;
                    console.log('resolving all users')
                    return getUsers()
                }
            }
        }
    })
});

// Middlewares

const loggingMiddleware = (req, res, next) => {
    const start = Date.now();
    console.log('Start profiling....')

    const currentGetUsers = getUsersCalls
    const currentGetPosts = getPostsByUserIdCalls
    const currentGetComments = getCommentsByPostCalls
    const currentGetUser = getUserByIdCalls
    const currentResolveCounts = { ...resolveCallCounts }

    res.on('finish', () => {
        const duration = Date.now() - start;
        const queryCounts = {
            users: getUsersCalls - currentGetUsers,
            posts: getPostsByUserIdCalls - currentGetPosts,
            comments: getCommentsByPostCalls - currentGetComments,
            user: getUserByIdCalls - currentGetUser
        };

        const resolveCounts = {
            users: resolveCallCounts.users - currentResolveCounts.users,
            posts: resolveCallCounts.posts - currentResolveCounts.posts,
            postAuthor: resolveCallCounts.postAuthor - currentResolveCounts.postAuthor,
            postComments: resolveCallCounts.postComments - currentResolveCounts.postComments,
            commentAuthor: resolveCallCounts.commentAuthor - currentResolveCounts.commentAuthor
        };

        const total = queryCounts.users + queryCounts.posts + queryCounts.comments + queryCounts.user
        const totalResolves = resolveCounts.users + resolveCounts.posts + resolveCounts.postAuthor + resolveCounts.postComments + resolveCounts.commentAuthor

        console.log(`[${new Date().toISOString()}] ${req.method} ${req.path} - IP: ${req.ip} - Status: ${res.statusCode} - ${duration}ms`);
        console.log(`SQL Queries - Users: ${queryCounts.users}, Posts: ${queryCounts.posts}, Comments: ${queryCounts.comments}, User: ${queryCounts.user}, Total: ${total}`);
        console.log(`Resolve calls - Users: ${resolveCounts.users}, Posts: ${resolveCounts.posts}, PostAuthor: ${resolveCounts.postAuthor}, PostComments: ${resolveCounts.postComments}, CommentAuthor: ${resolveCounts.commentAuthor}, Total: ${totalResolves}`);
    });
    next();
};

// Create express app

const app = express();
app.use(loggingMiddleware)
app.use(bodyParser.json())

const persistedQueryMiddleware = async (req, res, next) => {
    try {
        const documentId = req.body?.documentId;
        console.log(req.body)
        if (typeof documentId !== "string") {
            throw new Error("documentId is not a string");
        }
        const document = documents[documentId].query;
        if (!document) {
            throw new Error("document with given identifier not found");
        }
        delete req.body.documentId;
        req.body.query = document;
        console.log('continuing with query')
        return next();
    } catch (e) {
        next(e);
    }
}

app.all(
    '/graphql',
    persistedQueryMiddleware,
    createHandler({
        schema: schema,
        validationRules: [
            depthLimit(maxQueryDepth)
        ],
        context: () => ({
            loaders: {
                getPostsByUserId: new DataLoader(getPostsByUserIdBatch),
                getCommentsByPost: new DataLoader(getCommentsByPostBatch),
                getUserById: new DataLoader(getUserByIdBatch),
            }
        })
    })
)

app.get('/', (_req, res) => {
    res.type('html');
    res.end(ruruHTML({ endpoint: '/graphql' }));
});

app.listen(5000)
console.log("Running GraphQL server at http://localhost:5000/graphql")

/* Example
{
  users {
    ...getPosts
  }
}

fragment getPosts on User{
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
*/
/*
[2026-01-16T17:46:31.759Z] POST /graphql - IP: ::1 - Status: 200 - 133ms
SQL Queries - Users: 1, Posts: 1, Comments: 1, User: 0, Total: 3
Resolve calls - Users: 1, Posts: 10, PostAuthor: 0, PostComments: 1009, CommentAuthor: 0, Total: 1020
*/