open GraphqlHooksQuery;
open Util;

module AllPostsQuery = [%graphql
  {|
    query allPosts($first: Int!, $skip: Int!) {
      allPosts(orderBy: createdAt_DESC, first: $first, skip: $skip) {
        id
        title
        votes
        url
        createdAt
      }
      _allPostsMeta {
        count
      }
    }
  |}
];

[@react.component]
let make = () => {
  let (skip, setSkip) = React.useState(_ => 0);

  let ({response, loading}, refetch) =
    useQuery(
      ~variables=AllPostsQuery.makeVariables(~skip, ~first=10, ()),
      ~updateData=[%bs.raw
        {|
          (prevResult, result) => ({
            ...result,
            allPosts: [...prevResult.allPosts, ...result.allPosts],
          })
        |}
      ],
      AllPostsQuery.definition,
    );

  let handleUpdate = first =>
    refetch(~variables=AllPostsQuery.makeVariables(~skip=0, ~first, ()), ())
    |> ignore;

  switch (response) {
  | Loading => <div> "Loading"->s </div>
  | NoData => <div> "No Data"->s </div>
  | Data(data) =>
    let areMorePosts =
      Array.length(data##allPosts) < data##_allPostsMeta##count;

    <>
      <Submit
        onSubmission={() => handleUpdate(Array.length(data##allPosts))}
      />
      <section>
        <ul>
          {data##allPosts
           |> Js.Array.mapi((post, index) => {
                let votes =
                  switch (post##votes) {
                  | Some(votes) => votes
                  | None => 0
                  };

                <li key=post##id>
                  <div>
                    <span> {((index + 1)->string_of_int ++ ".")->s} </span>
                    <a href=post##url> {post##title->s} </a>
                    <PostUpvoter
                      id=post##id
                      votes
                      onUpdate={() =>
                        handleUpdate(Array.length(data##allPosts))
                      }
                    />
                  </div>
                </li>;
              })
           |> ate}
        </ul>
        {areMorePosts
           ? <button onClick={_ => setSkip(_ => skip + 10)}>
               {loading ? "Loading..." : "Show More"}->s
             </button>
           : React.null}
      </section>
    </>;
  | Error(error) =>
    GraphqlHooksTypes.(
      switch (error) {
      | GraphQLErrors(errors) =>
        errors
        |> Array.mapi((idx, error) =>
             <p key={idx->string_of_int}> error.message->s </p>
           )
        |> ate
      | HttpError(error) => <div> error.body->s </div>
      | FetchError(_error) => <div> "fetch error"->s </div>
      }
    )
  };
};