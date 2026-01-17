// seed.js
import sql from "./db.js";
// Run this script to insert fake data into the table

async function main() {
    await sql`
        DELETE FROM comments;
  `;

    await sql`
        DELETE FROM posts;
  `;

    await sql`
        DELETE FROM users;
  `;

    // users
    const users = await sql`
    INSERT INTO users (name)
    SELECT 'user-' || g
    FROM generate_series(1, 10) g
    RETURNING id
  `;

    // make user 1 heavy
    const heavyUser = users[0].id;

    // 1000 posts for user 1
    const posts = await sql`
    INSERT INTO posts (title, body, user_id)
    SELECT 
      'post-' || g,
      'body-' || g,
      ${heavyUser}
    FROM generate_series(1, 1000) g
    RETURNING id
  `;

    // one normal post for others
    for (let i = 1; i < users.length; i++) {
        await sql`
      INSERT INTO posts (title, body, user_id)
      VALUES ('small-post', 'body', ${users[i].id})
    `;
    }

    // pick one post to be heavy
    const heavyPost = posts[0].id;

    // 1000 comments on one post
    await sql`
    INSERT INTO comments (body, post_id, user_id)
    SELECT
      'comment-' || g,
      ${heavyPost},
      ${heavyUser}
    FROM generate_series(1, 1000) g
  `;

    console.log("Seed complete");
    await sql.end();
}

main();
