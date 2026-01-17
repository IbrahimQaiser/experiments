import { createHash } from 'crypto'

const queries = {
    "getPosts": `{
              users {
                name
                posts {
                  id
                  title
                  body
                }
              }
            }`,
    "getUsers": `
    {
        users {
            name
            id
        }
    }
    `
}

export const documents = {}

for (let key of Object.keys(queries)) {
    const hash = createHash('sha256').update(queries[key]).digest('hex')
    documents[hash] = {
        hash: hash,
        name: key,
        query: queries[key]
    }
}