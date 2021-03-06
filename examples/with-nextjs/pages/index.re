%raw
"require('../styles/main.css')";

open GraphqlHooksQuery;
open Util.ReactStuff;

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
let default = () => {
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

  <MainLayout>
    {switch (response) {
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
           <Text.Ul>
             {data##allPosts
              |> Js.Array.mapi((post, index) => {
                   let votes =
                     switch (post##votes) {
                     | Some(votes) => votes
                     | None => 0
                     };

                   <Text.Li key=post##id>
                     <div>
                       <span className="mr-1">
                         {((index + 1)->string_of_int ++ ".")->s}
                       </span>
                       <Text.A href=post##url> {post##title->s} </Text.A>
                       <PostUpvoter
                         id=post##id
                         votes
                         onUpdate={() =>
                           handleUpdate(Array.length(data##allPosts))
                         }
                       />
                     </div>
                   </Text.Li>;
                 })
              |> ate}
           </Text.Ul>
           {areMorePosts
              ? <button
                  className="bg-blue-400 hover:bg-blue-700 text-white py-1 px-2 rounded focus:outline-none focus:shadow-outline text-sm"
                  onClick={_ => setSkip(_ => skip + 10)}>
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
     }}
  </MainLayout>;
};