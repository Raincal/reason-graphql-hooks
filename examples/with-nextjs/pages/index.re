%raw
"require('../styles/main.css')";

open GraphqlHooks.Types;
open Util.ReactStuff;

module AllPostsQueryConfig = [%graphql
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

module AllPostsQuery = GraphqlHooksQuery.Make(AllPostsQueryConfig);

[@react.component]
let default = () => {
  let (skip, setSkip) = React.useState(_ => 0);

  let ({response}, refetch) =
    AllPostsQuery.use(
      ~variables=AllPostsQueryConfig.make(~skip, ~first=10, ())##variables,
      ~updateData=
        (prevResult, result) => [%js.deep
          result["allPosts"].replace(
            Belt.Array.concat(prevResult##allPosts, result##allPosts),
          )],
      (),
    );

  let handleUpdate = first =>
    refetch(
      ~variables=AllPostsQueryConfig.make(~skip=0, ~first, ())##variables,
      (),
    )
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
                  "Show More"->s
                </button>
              : React.null}
         </section>
       </>;
     | Error(error) =>
       switch (error) {
       | GraphQLErrors(errors) =>
         errors
         |> Array.mapi((idx, error) =>
              <p key={idx->string_of_int}> {error##message->s} </p>
            )
         |> ate
       | HttpError(error) => <div> {error##body->s} </div>
       | FetchError(_error) => <div> "fetch error"->s </div>
       }
     }}
  </MainLayout>;
};