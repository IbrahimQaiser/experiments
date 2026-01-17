import postgres from 'postgres'

const sql = postgres(`postgres://postgres:dev@0.0.0.0:5432/postgres?sslmode=disable`, {})

// Create tables once connection is established (if they do not exist)


try {
    await sql`
CREATE TABLE IF NOT EXISTS users(
    id BIGSERIAL PRIMARY KEY,
    name TEXT NOT NULL
);`

    await sql`CREATE TABLE IF NOT EXISTS posts(
    id BIGSERIAL PRIMARY KEY,
    title TEXT NOT NULL,
    body TEXT NOT NULL,
    user_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE
);`

    await sql`CREATE TABLE IF NOT EXISTS comments(
    id BIGSERIAL PRIMARY KEY,
    body TEXT NOT NULL,
    post_id BIGINT NOT NULL REFERENCES posts(id) ON DELETE CASCADE,
    user_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE
);`
} catch (e) {
    console.log('Error while creating tables:', e)
}

export let getUsersCalls = 0
export let getPostsByUserIdCalls = 0
export let getCommentsByPostCalls = 0
export let getUserByIdCalls = 0

export const getUsers = async () => {
    getUsersCalls++
    const users = await sql`SELECT * FROM users;`
    return users
}

export const getPostsByUserIdBatch = async (userIds) => {
    getPostsByUserIdCalls++
    const result = await sql`SELECT * FROM posts WHERE user_id IN ${sql(userIds)};`
    // Dataloader expects that if 10 keys are passed in the input array, 10 values should be returned
    // This query fetches posts of all users but does not group them, one way to group them is by using a separate map
    const mp = new Map();
    for (let row of result) {
        if (!mp.has(row.user_id)) {
            mp.set(row.user_id, [])
        }
        mp.get(row.user_id).push(row)
    }
    return userIds.map(id => mp.get(id) || [])
}

export const getCommentsByPostBatch = async (postIds) => {
    getCommentsByPostCalls++
    const result = await sql`SELECT * FROM comments WHERE post_id IN ${sql(postIds)};`
    // Similar to the above function, group the comments manually by using post_id as key
    const mp = new Map();
    for (let row of result) {
        if (!mp.has(row.post_id)) {
            mp.set(row.post_id, [])
        }
        mp.get(row.post_id).push(row)
    }
    return postIds.map(id => mp.get(id) || [])
}

export const getUserByIdBatch = async (ids) => {
    getUserByIdCalls++
    const u = await sql`SELECT * FROM users WHERE id IN ${sql(ids)}`
    return u
}

export default sql